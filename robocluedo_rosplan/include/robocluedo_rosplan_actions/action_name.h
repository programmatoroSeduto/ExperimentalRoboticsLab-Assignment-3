
/********************************************//**
*  
* @file ???.h
* @brief ROSPlan action implementation
* 
* @authors Francesco Ganci
* @version v1.0
* 
***********************************************/

#ifndef __H_ACTION_NAME_H__
#define __H_ACTION_NAME_H__

#define ROSPLAN_ACTION_NAME "action_name"

#include "ros/ros.h"
#include "rosplan_action_interface/RPActionInterface.h"
#include "knowledge_base_tools/feedback_manager.h"
#include "rosplan_dispatch_msgs/ActionDispatch.h"
// #include "robocluedo_rosplan_msgs/.h"

namespace KCL_rosplan 
{

class RP_rcl_action_name : public RPActionInterface
{
public:
	
	/** class constructor */
	RP_rcl_action_name( ros::NodeHandle& nh_ );
	
	/** class destructor */
	~RP_rcl_action_name( );
	
	/** ROSPlan concrete callback */
	bool concreteCallback( const rosplan_dispatch_msgs::ActionDispatch::ConstPtr& msg );

private:
	
	/// node handle
	ros::NodeHandle& nh;
	
	/// action feedback manager (as object instance)
	action_feedback_manager fb;
	
	/// name of the implemented action
	std::string action_name = ROSPLAN_ACTION_NAME;
};

}

#endif
