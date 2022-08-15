
/********************************************//**
*  
* @file move_to_center.cpp
* @brief ROSPlan action implementation as ROS node
* 
* @authors Francesco Ganci
* @version v1.0
* 
* @see feedback_manager.h
* 
***********************************************/

#include "ros/ros.h"
#include "knowledge_base_tools/feedback_manager.h"
#include "robocluedo_rosplan_actions/move_to_center.h"

#include <signal.h>

void shut_msg( int sig )
{
	ROS_INFO_STREAM("["<<ROSPLAN_ACTION_NAME<<"] "<<"stopping...");
	ros::shutdown( );
}

int main(int argc, char **argv) 
{
	ros::init(argc, argv, ROSPLAN_ACTION_NAME, 
		ros::init_options::AnonymousName | ros::init_options::NoSigintHandler );
	signal(SIGINT, shut_msg);
	
	ros::NodeHandle nh("~");
	
	ROS_INFO_STREAM("["<<ROSPLAN_ACTION_NAME<<"] "<<"starting...");
	KCL_rosplan::RP_rcl_move_to_center ac( nh );
	
	ROS_INFO_STREAM("["<<ROSPLAN_ACTION_NAME<<"] "<<"ready");
	ac.runActionInterface( );
	
	return 0;
}
