#! /usr/bin/env python

'''

@file move_base_nav.py

@brief navigation using MoveBase and the navigation stack

@author Francesco Ganci

'''

import rospy
import os
from std_srvs.srv import SetBool, SetBoolRequest, SetBoolResponse
from std_srvs.srv import Empty, EmptyRequest, EmptyResponse
from geometry_msgs.msg import Point
from tf import transformations
import math
from nav_msgs.msg import Odometry
from move_base_msgs.msg import MoveBaseActionGoal
from actionlib_msgs.msg import GoalID



active_ = False
''' the activity status of the node. 
'''



service_move_base_switch = "/nav_stack_switch"
''' name of the service to trigger the state of the node
'''

srv_move_base_switch = None
''' service handle for :any:`service_move_base_switch`
'''

def move_base_switch( data ):
	''' used for switching the activity status of the node. 
	
	Arguments:
		data (SetBool) :
			the boolean SetBool.data inside the message is the new activity status
	
	Note:
		before calling the service, make sure the three ros parameters
		'des_pos_x' , 'des_pos_y' and 'des_yaw' have been set.
	'''
	
	global active_
	global desired_position_, desired_yaw_
	
	res = SetBoolResponse( )
	res.success = True
	res.message = ""
	
	# check the activity status
	if active_ and not data.data:
		# rospy.loginfo( "(move_base_nav ) move_base navigation is OFF" )
		active_ = False
		change_state( 0 )
		
	elif not active_ and data.data:
		#rospy.loginfo( "(move_base_nav ) move_base navigation is ON" )
		active_ = True
		
	elif active_ and data.data:
		#rospy.loginfo( "(move_base_nav ) switching target" )
		pass
	
	else:
		res.message = "trying to disable something already off ..." 
	
	if active_:
		desired_position_.x = rospy.get_param( "des_pos_x" )
		desired_position_.y = rospy.get_param( "des_pos_y" )
		desired_yaw_ = rospy.get_param( "des_yaw" )
		
		#rospy.loginfo( f"(move_base_nav ) new target is (x={desired_position_.x}, y={desired_position_.y}, yaw={desired_yaw_})" )
	
	return res



service_move_base_signal = "/nav_stack_signal"
''' the node assumes that some other node implements this client, used
	for sending the signal when the robot reaches the objective. 
'''

cl_move_base_signal = None
''' (service client handle) client for :any:`service_move_base_signal`
'''

def send_signal( ):
	''' send a signal through the service :any:`service_move_base_signal`
		if available. 
	'''
	global cl_move_base_signal
	global service_move_base_signal
	
	if cl_move_base_signal == None:
		#rospy.loginfo( f"(move_base_nav ) TRYING client {service_move_base_signal} ... " )
		rospy.sleep(rospy.Duration(1))
		cl_move_base_signal = rospy.ServiceProxy( service_move_base_signal, Empty )
		if cl_move_base_signal == None:
			#rospy.loginfo( f"(move_base_nav ) unable to connect with {service_move_base_signal} -- retrying later..." )
			return
			
		else:
			#rospy.loginfo( f"(move_base_nav ) FOUND SERVICE {service_move_base_signal}" )
	
	if cl_move_base_signal != None:
		try:
			cl_move_base_signal( )
		except rospy.ServiceException:
			#rospy.loginfo( f"(move_base_nav ) unable to connect with {service_move_base_signal} -- service call failed" )
			cl_move_base_signal = None



service_head_orient_switch = "/head_orient_switch"
''' service name for the head orientation
'''

cl_head_orient_switch = None
''' (client handle) client to enable and disable the head orientation
'''



# current values

position_ = Point( )
''' the current position of the robot, from the odometry
'''

yaw_ = 0.0
''' the orientation of the robot about the 'z' axis, from the odometry
''' 

# target

desired_position_ = Point( )
''' the objective position for move_base. obtained from the parameters
	'des_pos_x' and 'des_pos_y'
''' 
desired_position_.x = 0.0
desired_position_.y = 0.0
desired_position_.z = 0.0

desired_yaw_ = 0.0
''' the current orientation of the robot
'''

# error evaluation

err_pos = math.inf
''' teh error between the desired position and the current one.

Note:
	if the position is not measured, the distance will be always infinite. 
'''

err_yaw = math.pi/2.0
''' the error between the current orientation and the desired one. 

Note:
	its default value is PI/2, and also when the distance is not measured
'''

# thresholds

threshold_position_ = 0.35
''' the maximum allowed position error while reaching the target. 
'''

yaw_precision_ = math.pi / 9 
''' +/- 20 degree allowed
'''

yaw_precision_2_ = math.pi / 90
''' +/- 2 degree allowed
'''

# odometry measurement

topic_odometry = "/odom"
''' the node measures the position of the robot during the navigation.
'''

sub_odometry = None
''' (subscription handle) subscription to the odometry
'''

def normalize_angle( angle ):
	''' angle normalization between -PI and PI.
	
	Note:
		the parameter "angle" is the err_yaw in this implementation.
	'''
	if(math.fabs(angle) > math.pi):
		angle = angle - (2 * math.pi * angle) / (math.fabs(angle))
	return angle


def cbk_odometry( msg ):
	''' subscription callback for the odometry topic
	'''
	global active_
	global position_, yaw_
	global desired_position_, desired_yaw_
	global err_pos, err_yaw
	
	if active_:
		# position
		position_ = msg.pose.pose.position

		# yaw
		quaternion = (
			msg.pose.pose.orientation.x,
			msg.pose.pose.orientation.y,
			msg.pose.pose.orientation.z,
			msg.pose.pose.orientation.w)
		euler = transformations.euler_from_quaternion(quaternion)
		yaw_ = euler[2]
		
		# estimation of errors
		err_pos = math.sqrt(pow(desired_position_.y - position_.y, 2) + pow(desired_position_.x - position_.x, 2))
		err_yaw = normalize_angle(desired_yaw_ - yaw_)
		
	else:
		# set defaults
		err_pos = math.inf
		err_yaw = math.pi / 2.0



topic_move_base_goal = "/move_base/goal"
''' the topic for sending the target to the navigation stack
'''

pub_move_base_goal = None
''' (publisher handle) move base goal
'''

def move_base_send_target( x, y ):
	''' send a target to move base. 
	
	Parameters:
		x (float) : 
			the x coordinate
		y (float) : 
			the y coordinate
	'''
	global topic_move_base_goal, pub_move_base_goal
	
	msg = MoveBaseActionGoal( )
	msg.header.frame_id = "map"
	msg.goal.target_pose.header.frame_id = "map"
	
	msg.goal.target_pose.pose.position.x = x
	msg.goal.target_pose.pose.position.y = y
	msg.goal.target_pose.pose.position.z = 0.0
	msg.goal.target_pose.pose.orientation.x = 0.0
	msg.goal.target_pose.pose.orientation.y = 0.0
	msg.goal.target_pose.pose.orientation.z = 0.0
	msg.goal.target_pose.pose.orientation.w = 1.0
	
	pub_move_base_goal.publish( msg )
	rospy.sleep( rospy.Duration(1) )



topic_move_base_cancel = "/move_base/cancel"
''' name of the topic for cancelling the navigation request
'''

pub_move_base_cancel = None
''' (publisher handle) the publisher for cancelling the request
'''

def move_base_cancel_goal( ):
	''' send a cancellation request to move_base
	'''
	global topic_move_base_cancel, pub_move_base_cancel
	
	msg = GoalID( )
	
	pub_move_base_cancel.publish( msg )
	rospy.sleep( rospy.Duration(1) )



state_ = 0
''' the current state of the node
'''

def change_state( state ):
	''' state transition from the current one to the one in the argument.
	''' 
	global state_ 
	global cl_head_orient_switch
	global desired_position_
	global state_description
	
	state_ = state
	
	if state_ == 0:   # -- idle
		state_description = "idle -- waiting for a target"
		cl_head_orient_switch( False )
		move_base_cancel_goal( )
	
	elif state_ == 1: # -- move_base planning
		state_description = "motion planning -- sending the request to move_base"
		cl_head_orient_switch( False )
		move_base_cancel_goal( )
		move_base_send_target( desired_position_.x, desired_position_.y )
	
	elif state_ == 2: # -- move_base motion
		state_description = "navigation -- going towards the target"
		cl_head_orient_switch( False )
	
	elif state_ == 3: # -- head_orientation behaviour
		state_description = "head orientation"
		move_base_cancel_goal( )
		cl_head_orient_switch( True )
	
	elif state_ == 4: # -- send signal (end of the motion)
		state_description = "SUCCESS. sending signal"
		cl_head_orient_switch( False )
		move_base_cancel_goal( )
		
	
	else:
		#rospy.logwarn( f"(move_base_nav ) WARNING: unknown state {state_}" )
		change_state( 0 )



replan_time = 30 # seconds
''' the planner, in case the robot is requiring too much time for reaching
	the point, can do a replanning request
'''

working_rate = 10 # Hz
''' the working rate is the maximum update frequency of this node. the status
	is checked with a frequency of 'working_rate' Hz.
'''

state_description = ""
''' just a message to make clear the current status of the node
'''

def main( ):
	''' state machine implementation
	'''
	global active_, state_
	global err_pos, err_yaw
	global threshold_position_, yaw_precision_2_
	global replan_time, working_rate
	global state_description
	
	'''
	while not rospy.is_shutdown():
		if active_:
			rospy.loginfo( "ON" )
		else:
			rospy.loginfo( "OFF" )
	'''
	
	'''
	rospy.spin( )
	'''
	
	r = rospy.Rate( working_rate )
	elapsed_time = 0.0
	time_unit = rospy.Duration( 1/working_rate )
	prev_output = ""
	while not rospy.is_shutdown( ):
		
		# wait (in any case)
		r.sleep( )
		
		# check active status
		if not active_:
			continue
		
		# update the goal
		desired_position_.x = rospy.get_param( "des_pos_x" )
		desired_position_.y = rospy.get_param( "des_pos_y" )
		desired_yaw_ = rospy.get_param( "des_yaw" )
		
		# run the state machine
		if state_ == 0:   # -- idle
			if err_pos > threshold_position_:
				change_state( 1 )
			elif err_yaw > yaw_precision_2_ :
				change_state( 2 )
		
		elif state_ == 1: # -- move_base planning
			elapsed_time = 0.0
			change_state( 2 )
		
		elif state_ == 2: # -- move_base motion
			elapsed_time = elapsed_time + time_unit.to_sec()
			
			if elapsed_time > replan_time:
				#rospy.loginfo( f"(move_base_nav ) replanning move_base (timer expired, max {replan_time})" )
				change_state( 1 ) # -- replanning
				continue
			
			if err_pos < threshold_position_:
				if err_yaw > yaw_precision_2_:
					change_state( 3 ) # -- orientation
				else:
					change_state( 4 ) # -- end of the task
		
		elif state_ == 3: # -- head_orientation behaviour
			if err_yaw <= yaw_precision_2_:
				change_state( 4 )
		
		elif state_ == 4: # -- send signal (end of the motion)
			send_signal( )
			change_state( 0 )
		
		else:
			#rospy.logwarn( f"(move_base_nav ) WARNING: unknown state {state_}" )
			change_state( 0 )
		
		# output : current status
		if prev_output != state_description:
			#rospy.loginfo( f"(move_base_nav ) current state = {state_} ({state_description})" )
			prev_output = state_description



def on_shut( ):
	rospy.loginfo( f"(move_base_nav ) closing..." )



if __name__ == "__main__":
	rospy.init_node( "move_base_nav" )
	rospy.on_shutdown( on_shut )
	
	# rospy.loginfo( f"(move_base_nav )" )
	
	#rospy.loginfo( f"(move_base_nav ) starting..." )
	rospy.sleep(rospy.Duration(2))
	
	if not rospy.has_param( "des_pos_x" ) or not rospy.has_param( "des_pos_y" ) or not rospy.has_param( "des_yaw" ):
		#rospy.logerr( "(move_base_nav ) ERROR: parameters not found in the parameter server!" )
		
		rospy.shutdown( )
		rospy.sleep(rospy.Duration(1))
		
		os.exit( )
	
	#rospyloginfo( f"(move_base_nav ) service {service_move_base_switch} ... " )
	srv_move_base_switch = rospy.Service( service_move_base_switch, SetBool, move_base_switch )
	#rospy.loginfo( f"(move_base_nav ) service {service_move_base_switch} ... OK" )
	
	#rospy.loginfo( f"(move_base_nav ) subscription: {topic_odometry} ... " )
	sub_odometry = rospy.Subscriber( topic_odometry , Odometry, cbk_odometry )
	rospy.sleep(rospy.Duration(1))
	#rospy.loginfo( f"(move_base_nav ) subscription: {topic_odometry} ... OK" )
	
	#rospy.loginfo( f"(move_base_nav ) client: {service_head_orient_switch} ... " )
	cl_head_orient_switch = rospy.ServiceProxy( service_head_orient_switch, SetBool )
	rospy.sleep(rospy.Duration(1))
	#rospy.loginfo( f"(move_base_nav ) client: {service_head_orient_switch} ... OK" )
	
	#rospy.loginfo( f"(move_base_nav ) publisher: {topic_move_base_goal} ... " )
	pub_move_base_goal = rospy.Publisher( topic_move_base_goal, MoveBaseActionGoal, queue_size=10 )
	#rospy.loginfo( f"(move_base_nav ) publisher: {topic_move_base_goal} ... OK" )
	
	#rospy.loginfo( f"(move_base_nav ) publisher: {topic_move_base_cancel} ... " )
	pub_move_base_cancel = rospy.Publisher( topic_move_base_cancel, GoalID, queue_size=10 )
	#rospy.loginfo( f"(move_base_nav ) publisher: {topic_move_base_cancel} ... OK" )
	
	rospy.sleep(rospy.Duration(2))
	
	#rospy.loginfo( f"(move_base_nav ) ready" )
	main( )
