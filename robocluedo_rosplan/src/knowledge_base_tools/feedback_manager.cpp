
/********************************************//**
*  
* @file feedback_manager.cpp
* @brief additional informations from the PDDL action 
* 	implementations as topic
* 
* @authors Francesco Ganci
* @version v1.0
* 
* @see feedback_manager.h
* 
***********************************************/

#include "knowledge_base_tools/feedback_manager.h"




// === BASE METHODS === //

// class constructor
action_feedback_manager::action_feedback_manager( std::string act_name )
{
	this->action_name = act_name;
	
	// open the action feedback topic
	ROS_INFO_STREAM("["<<action_name<<"::feedback_manager] creating feedback publisher...");
	this->pub_action_feedback = nh.advertise<robocluedo_rosplan_msgs::ActionFeedback>( TOPIC_ACTION_FEEDBACK, 10 );
	ROS_INFO_STREAM("["<<action_name<<"::feedback_manager] OK");
}


// class destructor
action_feedback_manager::~action_feedback_manager( )
{
	// ...
}




// === FEEDBACK SYSTEM === //

// other failures
void action_feedback_manager::fb_failure( std::vector<diagnostic_msgs::KeyValue> parameters, std::string details )
{
	auto msg = this->build_msg( parameters, false, true, false, false, false, details );
	this->pub( msg );
}

// unspecified hardware failure
void action_feedback_manager::fb_hw_failure( std::vector<diagnostic_msgs::KeyValue> parameters, std::string details )
{
	auto msg = this->build_msg( parameters, false, true, true, false, false, details );
	this->pub( msg );
}

// hardware manipulation failure
void action_feedback_manager::fb_hw_manipulation_failure( std::vector<diagnostic_msgs::KeyValue> parameters, std::string details )
{
	auto msg = this->build_msg( parameters, false, true, true, false, true, details );
	this->pub( msg );
}

// hardware navigation failure
void action_feedback_manager::fb_hw_navigation_failure( std::vector<diagnostic_msgs::KeyValue> parameters, std::string details )
{
	auto msg = this->build_msg( parameters, false, true, true, true, false, details ) ;
	this->pub( msg );
}




// === PRIVATE METHODS === //

// build a message
robocluedo_rosplan_msgs::ActionFeedback action_feedback_manager::build_msg( 
		std::vector<diagnostic_msgs::KeyValue> parameters,
		bool goal_achieved,
		bool failure,
		bool hw_failure,
		bool failure_nav_system,
		bool failure_manipulation,
		std::string details )
{
	if( this->action_name == ACTION_NAME_UNKNOWN )
		ROS_WARN_STREAM("["<<action_name<<"::feedback_manager] WARNING: "
			<<"action name of the feedback manager is UNKNOWN");
	
	robocluedo_rosplan_msgs::ActionFeedback msg;
	
	// header
	msg.action_name = action_name;
	msg.parameters = parameters;
	
	// feedback properties
	msg.goal_achieved = goal_achieved;
	msg.failure = failure;
	msg.hw_failure = hw_failure;
	msg.failure_nav_system = failure_nav_system;
	msg.details = details;
	
	return msg;
}


// publish the feedback
void action_feedback_manager::pub( robocluedo_rosplan_msgs::ActionFeedback& msg )
{
	this->pub_action_feedback.publish( msg );
}
