#! /usr/bin/env python

'''

@brief find the hint associates to a received aruco id

@authors Francesco Ganci
@version 1.0.0

'''

import rospy
import os
from std_msgs.msg import Int32
from erl2.msg import ErlOracle
from exp_assignment3.srv import Marker, MarkerRequest, MarkerResponse


NODE_NAME = "aruco_decode"
''' ROS name of the node
'''




topic_system = "/oracle_hint"
''' topic through which transmit the meaning of the ID
'''

pub_system = None
''' (publisher handle) 
'''




service_oracle = "/original_oracle_hint"
''' channel for interrogat the Oracle about the meaning of the detected ID
'''

cl_oracle = None
''' (client handle)
'''




topic_aruco_detection = "/aruco_detected_ids"
''' topic from which the node received the IDs from the aruco detection unit
'''

sub_aruco_detection = None
''' (subscription handle) receive the IDs from ArUco detection
'''

def cbk_aruco_detection( idpack ):
	''' read a ArUco ID from the detection unti, find its meaning, and
	retransmit the associated hint to the system.
	
	Parameters:
		idpack (std_msgs/Int32):
			the ID from the detection unit, see idpack.data
	'''
	global pub_system
	global cl_oracle
	
	rospy.loginfo( f"({NODE_NAME} ) received ID={idpack.data}" )
	
	# ask the meaning to the Oracle
	ask = MarkerRequest( )
	ask.markerId = idpack.data
	
	oracle_res = None
	try:
		oracle_res = cl_oracle( ask )
	except rospy.ServiceException:
		rospy.loginfo( f"({NODE_NAME} ) ERROR: unable to contact the Oracle" )
		return
	
	rospy.loginfo( f"({NODE_NAME} ) oracle says (ID={oracle_res.oracle_hint.ID}, key={oracle_res.oracle_hint.key}, value={oracle_res.oracle_hint.value})" )
	
	# echo the message
	pub_system.publish( oracle_res.oracle_hint )




def on_shut( ):
	''' simple message saying that the node is going to close
	'''
	rospy.loginfo( f"(move_base_nav ) closing..." )




if __name__ == "__main__":
	
	rospy.init_node( NODE_NAME )
	rospy.on_shutdown( on_shut )
	
	# rospy.loginfo( f"({NODE_NAME} )" )
	
	rospy.loginfo( f"({NODE_NAME} ) starting ..." )
	rospy.sleep( rospy.Duration( 2 ) )
	
	rospy.loginfo( f"({NODE_NAME} subscription {topic_aruco_detection}) ... " )
	sub_aruco_detection = rospy.Subscriber( topic_aruco_detection, Int32, cbk_aruco_detection )
	rospy.sleep( rospy.Duration( 1 ) )
	rospy.loginfo( f"({NODE_NAME} subscription {topic_aruco_detection}) ... OK" )
	
	
	rospy.loginfo( f"({NODE_NAME} publisher {topic_system}) ... " )
	pub_system = rospy.Publisher( topic_system, ErlOracle, queue_size=1000 )
	rospy.sleep( rospy.Duration( 1 ) )
	rospy.loginfo( f"({NODE_NAME} publisher {topic_system}) ... OK" )
	
	
	rospy.loginfo( f"({NODE_NAME} client {service_oracle}) ... " )
	cl_oracle = rospy.ServiceProxy( service_oracle, Marker )
	rospy.sleep( rospy.Duration( 1 ) )
	if cl_oracle == None:
		rospy.loginfo( f"({NODE_NAME} ) ERROR: cl_oracle==None -- closing ... " )
		os.exit( )
	rospy.loginfo( f"({NODE_NAME} client {service_oracle}) ... OK" )
	
	rospy.sleep( rospy.Duration( 2 ) )
	
	rospy.loginfo( f"({NODE_NAME} ) ready" )
	rospy.spin( )
