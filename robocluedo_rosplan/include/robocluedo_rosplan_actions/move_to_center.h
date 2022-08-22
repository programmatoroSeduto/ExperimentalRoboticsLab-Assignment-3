
/********************************************//**
*  
* @file move_to_center.h
* @brief ROSPlan action implementation
* 
* @todo join this action with \ref RP_rcl_move_to
* 
* @authors Francesco Ganci
* @version v1.0
* 
* @see move_to.h
* 
***********************************************/

#ifndef __H_MOVE_TO_CENTER_H__
#define __H_MOVE_TO_CENTER_H__

#define ROSPLAN_ACTION_NAME "move_to_center"

#include "ros/ros.h"
#include "rosplan_action_interface/RPActionInterface.h"
#include "knowledge_base_tools/feedback_manager.h"
#include "rosplan_dispatch_msgs/ActionDispatch.h"
#include "robocluedo_rosplan_msgs/NavigationCommand.h"

#define SRV_NAVIGATION "/robocluedo/navigation_command"

namespace KCL_rosplan 
{

class RP_rcl_move_to_center : public RPActionInterface
{
public:
	
	/** class constructor */
	RP_rcl_move_to_center( ros::NodeHandle& nh_ );
	
	/** class destructor */
	~RP_rcl_move_to_center( );
	
	/** ROSPlan concrete callback */
	bool concreteCallback( const rosplan_dispatch_msgs::ActionDispatch::ConstPtr& msg );

private:
	
	/// node handle
	ros::NodeHandle& nh;
	
	/// action feedback manager (as object instance)
	action_feedback_manager fb;
	
	/// client -- navigation service
	ros::ServiceClient cl_navigation;
	
	/// name of the implemented action
	std::string action_name = ROSPLAN_ACTION_NAME;
};

}

#endif
