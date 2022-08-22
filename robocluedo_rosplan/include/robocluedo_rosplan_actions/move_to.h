
/********************************************//**
*  
* @file move_to.h
* @brief ROSPlan action implementation
* 
* This nodE implements the action (move-to ?from ?to) telling the robot to 
* move from one "marker" to the other one. 
* 
* Very important to point out that, at this level, the system performs a
* topological localization, leaving the service implementing the navigaiton
* to assign the cartesian coordinates to the waypoints. 
* 
* @authors Francesco Ganci
* @version v1.0
* 
* @note as other actions do, the move_to, in order to carry out the navigation,
* 	assumes some other node implemented a service defined by this package, and 
* 	then the node tries to connect to that service. 
* 
***********************************************/

#ifndef __H_MOVE_TO_H__
#define __H_MOVE_TO_H__

#define ROSPLAN_ACTION_NAME "move_to"

#include "ros/ros.h"
#include "rosplan_action_interface/RPActionInterface.h"
#include "knowledge_base_tools/feedback_manager.h"
#include "rosplan_dispatch_msgs/ActionDispatch.h"
#include "robocluedo_rosplan_msgs/NavigationCommand.h"

#define SRV_NAVIGATION "/robocluedo/navigation_command"

namespace KCL_rosplan 
{

class RP_rcl_move_to : public RPActionInterface
{
public:
	
	/** class constructor */
	RP_rcl_move_to( ros::NodeHandle& nh_ );
	
	/** class destructor */
	~RP_rcl_move_to( );
	
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
