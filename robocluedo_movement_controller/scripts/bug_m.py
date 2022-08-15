#! /usr/bin/env python

''' 
follow the walls using wall follow
when near enough to the point to reach, go to point
and that's all!

the node uses other nodes exploiting the laser sensors
'''

import rospy
import os
import time
# import ros message
from geometry_msgs.msg import Point
from sensor_msgs.msg import LaserScan
from nav_msgs.msg import Odometry
from tf import transformations
# import ros service
from std_srvs.srv import *
from geometry_msgs.msg import Twist

import math

active_ = False

pub = None
srv_client_go_to_point_ = None
srv_client_wall_follower_ = None
srv_client_user_interface_ = None
srv_client_head_orientation_ = None
yaw_ = 0
yaw_error_allowed_ = 5 * (math.pi / 180)  # 5 degrees
position_ = Point()
desired_position_ = Point()
desired_position_.x = rospy.get_param('des_pos_x')
desired_position_.y = rospy.get_param('des_pos_y')
desired_position_.z = 0
desired_yaw_ = rospy.get_param('des_yaw')
yaw_precision_ = math.pi / 9  # +/- 20 degree allowed
yaw_precision_2_ = math.pi / 90  # +/- 2 degree allowed
regions_ = None
state_desc_ = ['Go to point', 'wall following', 'target reached']
state_ = 0
# 0 - go to point
# 1 - wall following
# 2 - idle
# 3 - head orientation

# callbacks


def clbk_odom(msg):
	global position_, yaw_

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


def clbk_laser(msg):
	global regions_
	regions_ = {
		'right':  min(min(msg.ranges[0:143]), 10),
		'fright': min(min(msg.ranges[144:287]), 10),
		'front':  min(min(msg.ranges[288:431]), 10),
		'fleft':  min(min(msg.ranges[432:575]), 10),
		'left':   min(min(msg.ranges[576:719]), 10),
	}


def change_state(state):
	global state_, state_desc_
	global srv_client_wall_follower_, srv_client_go_to_point_, srv_client_head_orientation_
	global srv_client_user_interface_
	
	state_ = state
	
	# log = "state changed: %s" % state_desc_[state]
	# rospy.loginfo(log)
	
	if state_ == 0:
		srv_client_go_to_point_(True)
		srv_client_wall_follower_(False)
		srv_client_head_orientation_(False)
		
	elif state_ == 1:
		srv_client_go_to_point_(False)
		srv_client_wall_follower_(True)
		srv_client_head_orientation_(False)
		
	elif state_ == 2:
		srv_client_go_to_point_(False)
		srv_client_wall_follower_(False)
		srv_client_head_orientation_(False)
		
		twist_msg = Twist()
		twist_msg.linear.x = 0
		twist_msg.angular.z = 0
		pub.publish(twist_msg)
		
		if (srv_client_user_interface_ == None):
			srv_client_user_interface_ = rospy.ServiceProxy('/bug_m_signal', Empty)
		if srv_client_user_interface_ != None:
			try:
				srv_client_user_interface_()
			except rospy.ServiceException:
				srv_client_user_interface_ = None
		
	elif state_ == 3:
		srv_client_go_to_point_(False)
		srv_client_wall_follower_(False)
		srv_client_head_orientation_(True)
	
	elif state_ == 4:
		srv_client_go_to_point_(False)
		srv_client_wall_follower_(False)
		srv_client_head_orientation_(False)


def normalize_angle(angle):
	if(math.fabs(angle) > math.pi):
		angle = angle - (2 * math.pi * angle) / (math.fabs(angle))
	return angle


def bug_switch(req):
	global active_
	global srv_client_wall_follower_, srv_client_go_to_point_, srv_client_head_orientation_
	
	active_ = req.data
	
	if not active_:
		rospy.loginfo("(bug_m) OFF")
		change_state(2)
	else:
		rospy.loginfo("(bug_m) ON ")
	
	res = SetBoolResponse( )
	res.success = True
	res.message = 'Done!'
	
	return res


def main():
	global regions_, position_, desired_position_, yaw_, yaw_error_allowed_
	global srv_client_go_to_point_, srv_client_wall_follower_, srv_client_user_interface_, srv_client_head_orientation_
	global pub
	global desired_yaw_, yaw_precision_, yaw_precision_2_
	global active_, state_

	rospy.init_node('bug0')
	
	rospy.loginfo("(bug_m) starting...")
	rospy.sleep(rospy.Duration(2))

	sub_laser = rospy.Subscriber('/scan', LaserScan, clbk_laser)
	sub_odom = rospy.Subscriber('/odom', Odometry, clbk_odom)
	pub = rospy.Publisher('/cmd_vel', Twist, queue_size=1)
	
	srv_client_go_to_point_ = rospy.ServiceProxy('/go_to_point_switch', SetBool)
	if( srv_client_go_to_point_ == None ):
		rospy.logwarn("(bug_m) srv_client_go_to_point_ == None")
		return
	
	srv_client_wall_follower_ = rospy.ServiceProxy('/wall_follower_switch', SetBool)
	if( srv_client_wall_follower_ == None ):
		rospy.logwarn("(bug_m) srv_client_wall_follower_ == None")
		return
	
	srv_client_head_orientation_ = rospy.ServiceProxy('/head_orient_switch', SetBool)
	if( srv_client_head_orientation_ == None ):
		rospy.logwarn("(bug_m) srv_client_head_orientation_ == None")
		return
	
	switch_srv = rospy.Service('/bug_switch', SetBool, bug_switch)
	'''
	try:
		rospy.wait_for_service('/bug_m_signal', timeout=60)
		srv_client_user_interface_ = rospy.ServiceProxy('/bug_m_signal', Empty)
		if( srv_client_user_interface_ == None ):
			rospy.logwarn("(bug_m) srv_client_user_interface_ == None")
			return
	except rospy.ROSException as e:
		rospy.logwarn("(bug_m) unable to contact to the provider server for /bug_switch")
		return
	'''
	srv_client_user_interface_ = rospy.ServiceProxy('/bug_m_signal', Empty)
	
	rospy.sleep(rospy.Duration(2))
	rospy.loginfo("(bug_m) ready")
	
	# initialize going to the point
	change_state(2)

	rate = rospy.Rate(20)
	while not rospy.is_shutdown():
		if regions_ == None:
			# rospy.loginfo(f"(bug_m) regions is none!")
			continue
		
		rate.sleep( )
		if not active_:
			# rospy.loginfo(f"(bug_m) is off")
			continue
		else:
			# rospy.loginfo(f"(bug_m) state {state_}")
			
			desired_position_.x = rospy.get_param('des_pos_x')
			desired_position_.y = rospy.get_param('des_pos_y')
			desired_yaw_ = rospy.get_param('des_yaw')
			err_pos = math.sqrt(pow(desired_position_.y - position_.y, 2) + pow(desired_position_.x - position_.x, 2))
			print( f"err_pos={err_pos}" )
			err_yaw = normalize_angle(desired_yaw_ - yaw_)
			print( f"err_yaw={err_yaw}" )
			print( f"front region = {regions_['front']}" )
			
			if state_ == 0:
				''' 
				go_to_point -- ON
				wall_follow -- OFF
				head orient -- OFF
				'''
				print("(bug_m) go to point")
				err_pos = math.sqrt(pow(desired_position_.y - position_.y, 2) + pow(desired_position_.x - position_.x, 2))
				
				if(err_pos < 0.3):
					change_state(3)
				'''
				elif regions_['front'] < 0.2:
					change_state(1)
				'''

			elif state_ == 1:
				''' 
				go_to_point -- OFF
				wall_follow -- ON
				head orient -- OFF
				'''
				print("(bug_m) wall follow")
				desired_yaw = math.atan2(desired_position_.y - position_.y, desired_position_.x - position_.x)
				err_yaw = normalize_angle(desired_yaw - yaw_)
				err_pos = math.sqrt(pow(desired_position_.y - position_.y, 2) + pow(desired_position_.x - position_.x, 2))

				if(err_pos < 0.3):
					change_state(3)
				if regions_['front'] > 1 and math.fabs(err_yaw) < 0.05:
					change_state(0)

			elif state_ == 2:
				''' 
				go_to_point -- OFF
				wall_follow -- OFF
				head orient -- OFF
				'''
				print("(bug_m) end")
				
				# se il robot ha già raggiunto il target, rimarrà fermo
				# altrimenti inizierà a muoversi
				# (l'approccio non è molto efficace coi piccoli movimenti)
				
				if(err_pos > 0.35):
					if regions_['front'] < 0.4:
						change_state(4)
					else:
						change_state(0)
				
				elif(err_yaw > yaw_precision_2_):
					if regions_['front'] < 0.4:
						change_state(4)
					else:
						change_state(3)
			
			elif state_ == 3:
				'''
				go_to_point -- OFF
				wall_follow -- OFF
				head orient -- ON
				
				change yaw
				'''
				print("(bug_m) change yaw")
				
				if(err_pos > 0.35):
					if regions_['front'] < 0.4:
						change_state(4)
					else:
						change_state(0)
				elif(err_yaw <= yaw_precision_2_):
					change_state(2)
			
			elif state_ == 4:
				'''
				go_to_point -- OFF
				wall_follow -- OFF
				head orient -- OFF
				
				go backwards
				'''
				print("(bug_m) go back")
				
				tw = Twist( )
				tw.linear.x = -0.08
				
				pub.publish(tw)
				
				if regions_['front'] > 0.4:
					if(err_pos > 0.35):
						change_state(0)
					else:
						change_state(3)


if __name__ == "__main__":
	main()
