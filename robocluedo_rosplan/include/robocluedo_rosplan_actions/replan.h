
/********************************************//**
*  
* @file replan.h
* @brief ROSPlan action implementation
* 
* @authors Francesco Ganci
* @version v1.0
* 
***********************************************/

#ifndef __H_REPLAN_H__
#define __H_REPLAN_H__

#define ROSPLAN_ACTION_NAME "replan"

#include "ros/ros.h"
#include "std_srvs/Empty.h"
#include "rosplan_action_interface/RPActionInterface.h"
#include "knowledge_base_tools/feedback_manager.h"
#include "rosplan_dispatch_msgs/ActionDispatch.h"

#define SRV_REPLAN "/replan"

namespace KCL_rosplan 
{

class RP_rcl_replan : public RPActionInterface
{
public:
	
	/** class constructor */
	RP_rcl_replan( ros::NodeHandle& nh_ );
	
	/** class destructor */
	~RP_rcl_replan( );
	
	/** ROSPlan concrete callback */
	bool concreteCallback( const rosplan_dispatch_msgs::ActionDispatch::ConstPtr& msg );

private:
	
	/// node handle
	ros::NodeHandle& nh;
	
	/// action feedback manager (as object instance)
	action_feedback_manager fb;
	
	/// client replan
	ros::ServiceClient cl_replan;
	
	/// name of the implemented action
	std::string action_name = ROSPLAN_ACTION_NAME;
};

}

#endif
