
/********************************************//**
*  
* @file feedback_manager.h
* @brief additional informations from the PDDL action 
* 	implementations as topic
* 
* @authors Francesco Ganci
* @version v1.0
* 
***********************************************/

#ifndef __H_FEEDBACK_MANAGER_H__
#define __H_FEEDBACK_MANAGER_H__

#ifndef ACTION_NAME_UNKNOWN
	#define ACTION_NAME_UNKNOWN "UNKNOWN"
#endif

#include "ros/ros.h"
#include "diagnostic_msgs/KeyValue.h"
#include "robocluedo_rosplan_msgs/ActionFeedback.h"

#include <string>
#include <vector>
#include <map>

#define TOPIC_ACTION_FEEDBACK "/robocluedo/action_feedback"

/*
ROS_INFO_STREAM("["<<action_name<<"::feedback_manager] "<<"");
ROS_WARN_STREAM("["<<action_name<<"::feedback_manager] WARNING: "<<"");
*/

/********************************************//**
 *  
 * \brief utilities for sending further infos to the
 * 	pipeline manager. 
 * 
 ***********************************************/
class action_feedback_manager
{
public:
	
	/// the name of the action
	std::string action_name;
	
	
	// === BASE METHODS === //
	
	/********************************************//**
	 *  
	 * \brief constructor of action_feedback_manager
	 * 
	 * this function opens the publisher and performs some other
	 * intialization. 
	 * 
	 ***********************************************/
	action_feedback_manager( std::string act_name = ACTION_NAME_UNKNOWN );
	
	/** class destructor */
	~action_feedback_manager( );
	
	
	// === FEEDBACK SYSTEM === //
	
	/** other failures
	 * 
	 * forinstance, can't reach a given service. 
	 * 
	 */
	void fb_failure( std::vector<diagnostic_msgs::KeyValue> parameters, std::string details );
	
	/// unspecified hardware failure
	void fb_hw_failure( std::vector<diagnostic_msgs::KeyValue> parameters, std::string details );
	
	/// hardware manipulation failure
	void fb_hw_manipulation_failure( std::vector<diagnostic_msgs::KeyValue> parameters, std::string details );
	
	/// hardware navigation failure
	void fb_hw_navigation_failure( std::vector<diagnostic_msgs::KeyValue> parameters, std::string details );
	
	
private:
	
	/// ROS node handle
	ros::NodeHandle nh;
	
	/// the publisher handle
	ros::Publisher pub_action_feedback;
	
	/********************************************//**
	 *  
	 * \brief build a action feedback message
	 * 
	 * @param parameters directly from the propertied of the action
	 * 
	 * @returns the feedback message ready to be sent
	 * 
	 ***********************************************/
	robocluedo_rosplan_msgs::ActionFeedback build_msg( 
		std::vector<diagnostic_msgs::KeyValue> parameters,
		bool goal_achieved = true,
		bool failure = false,
		bool hw_failure = false,
		bool failure_nav_system = false,
		bool failure_manipulation = false,
		std::string details = "" );
	
	/// publish the feedback
	void pub( robocluedo_rosplan_msgs::ActionFeedback& msg );
};

#endif
