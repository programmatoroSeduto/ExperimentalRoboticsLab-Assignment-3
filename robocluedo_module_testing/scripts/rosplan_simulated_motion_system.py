#! /usr/bin/env python

import rospy
from robocluedo_rosplan_msgs.srv import NavigationCommand, NavigationCommandRequest, NavigationCommandResponse
from robocluedo_rosplan_msgs.srv import ManipulationCommand, ManipulationCommandRequest, ManipulationCommandResponse


def simulated_navigation( cmd ):
	''' simulate the service for the navigation
	'''
	rospy.loginfo(f"navigation SIMULATED {cmd.waypoint}")
	return NavigationCommandResponse( success=True )


def simulated_manipulation( cmd ):
	''' simulate the service for the manipulation
	'''
	if cmd.home_position:
		rospy.loginfo("manipulation SIMULATED -> home position")
	else:
		rospy.loginfo("manipulation SIMULATED -> near the marker")
	return ManipulationCommandResponse( success=True )


def open_srv( srv_name, srv_type, srv_callback ):
	''' handful utility to open a service
	'''
	
	rospy.loginfo(f"(rosplan_simulated_motion_system) service: {srv_name} ...")
	srv_this = rospy.Service( srv_name, srv_type, srv_callback )
	rospy.sleep( rospy.Duration(0.75) )
	rospy.loginfo(f"(rosplan_simulated_motion_system) service: {srv_name} ... OK")
	
	return srv_this


def shut_msg( ):
	''' called at the shutdown of the node, just a message
	'''
	rospy.loginfo( "(simulated_motion_controller) stopping ... " )


if __name__ == "__main__":
	rospy.init_node( "simulated_motion_controller" )
	rospy.on_shutdown( shut_msg )
	
	rospy.loginfo("(rosplan_simulated_motion_system) starting...")
	open_srv( "/robocluedo/manipulator_command", ManipulationCommand, simulated_manipulation )
	open_srv( "/robocluedo/navigation_command", NavigationCommand, simulated_navigation )
	
	rospy.loginfo("(rosplan_simulated_motion_system) ready")
	rospy.spin()
