#! /usr/bin/env python

import rospy

from geometry_msgs.msg import Twist, Point
from nav_msgs.msg import Odometry
from std_srvs.srv import *
import math

from tf import transformations

yaw_precision_ = math.pi / 9  # +/- 20 degree allowed
''' yaw precision parameter
'''

yaw_precision_2_ = math.pi / 90  # +/- 2 degree allowed
''' yaw precision parameter
'''

active_ = False
''' general status of the node
'''

yaw_ = 0
''' current head orientation of the robot, in radiants
'''

state_ = 0
''' internal state of the robot
'''

desired_yaw_ = rospy.get_param('des_yaw')
''' the yaw to reach, from the parameter server
'''

# movement control parameters
kp_a = 3.0  # In ROS Noetic, it may be necessary to change the sign of this proportional controller
kp_d = 0.2
ub_a = 0.6
lb_a = -0.5
ub_d = 0.6

pub = None
''' cmd_vel publisher
'''



def head_orient_switch(req):
	''' node switch, service /head_orient_switch
	'''
	global active_, state_, pub
	
	active_ = req.data
	if not active_:
		twist_msg = Twist()
		twist_msg.linear.x = 0
		twist_msg.angular.z = 0
		pub.publish(twist_msg)
		state_ = 2
	
	res = SetBoolResponse()
	res.success = True
	res.message = 'Done!'
	return res


def clbk_odom(msg):
	''' read the current yaw from the odometry
	'''
	global yaw_
	
	# yaw
	quaternion = (
		msg.pose.pose.orientation.x,
		msg.pose.pose.orientation.y,
		msg.pose.pose.orientation.z,
		msg.pose.pose.orientation.w)
	euler = transformations.euler_from_quaternion(quaternion)
	yaw_ = euler[2]


def change_state(state):
	global state_
	state_ = state


def normalize_angle(angle):
	'''the function normalises the angle between -pi and pi
	'''
	if(math.fabs(angle) > math.pi):
		angle = angle - (2 * math.pi * angle) / (math.fabs(angle))
	return angle


def fix_yaw(des_pos):
	''' the robot tries to reach a  given yaw. 
	
	this function implements the state zero of the node.
	
	Returns:
		the absolute head orientation error
	'''
	global yaw_, pub, yaw_precision_2_, state_, desired_yaw_
	
	err_yaw = normalize_angle(desired_yaw_ - yaw_)
	
	# rospy.loginfo(err_yaw)
	
	twist_msg = Twist()
	if math.fabs(err_yaw) > yaw_precision_2_:
		twist_msg.angular.z = kp_a*err_yaw
		if twist_msg.angular.z > ub_a:
			twist_msg.angular.z = ub_a
		elif twist_msg.angular.z < lb_a:
			twist_msg.angular.z = lb_a

	pub.publish(twist_msg)
	
	return math.fabs(err_yaw)


def done( ):
	''' function called in state 2: stop the robot motion
	'''
	global pub
	
	twist_msg = Twist()
	twist_msg.linear.x = 0
	twist_msg.angular.z = 0
	
	pub.publish(twist_msg)


def main():
	global pub, active_, yaw_precision_2_, desired_yaw_, state_
	
	rospy.init_node('head_orientation')
	
	#rospy.loginfo("(head_orientation) starting...")
	rospy.sleep(rospy.Duration(2))
	
	pub = rospy.Publisher('/cmd_vel', Twist, queue_size=1)
	sub_odom = rospy.Subscriber('/odom', Odometry, clbk_odom)
	srv = rospy.Service('/head_orient_switch', SetBool, head_orient_switch)
	
	rospy.sleep(rospy.Duration(2))
	#rospy.loginfo("(head_orientation) ready")
	
	rate = rospy.Rate(20)
	while not rospy.is_shutdown():
		desired_yaw_ = rospy.get_param('des_yaw')
		
		if not active_:
			rate.sleep()
			continue
		else:
			if state_ == 0:
				if fix_yaw(desired_yaw_) <= yaw_precision_2_:
					change_state(1)
			
			elif state_ == 1:
				done( )
				change_state(2)
			
			elif state_ == 2:
				if normalize_angle(desired_yaw_ - yaw_) > yaw_precision_2_:
					change_state(0)
			
			else:
				#rospy.logerr('Unknown state!')
		
		rate.sleep()


if __name__ == '__main__':
	main()
