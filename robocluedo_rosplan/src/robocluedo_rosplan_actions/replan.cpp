
/********************************************//**
*  
* @file replan.cpp
* @brief ROSPlan action implementation
* 
* @authors Francesco Ganci
* @version v1.0
* 
* @see feedback_manager.h
* 
***********************************************/

#include "robocluedo_rosplan_actions/replan.h"

/*
# actionDispatch message

int32 action_id
int32 plan_id
string name
diagnostic_msgs/KeyValue[] parameters
float32 duration
float32 dispatch_time
*/

/*
ROS_INFO_STREAM("["<<action_name<<"] "<<"");
ROS_WARN_STREAM("["<<action_name<<"] WARNING: "<<"");


ROS_INFO_STREAM("["<<action_name<<"] "<<"client " << SRV_??? << " ...");
cl_??? = nh.serviceClient<???>( SRV_??? );
if( !cl_???.waitForExistence( ros::Duration( 60 ) ) )
{
	ROS_WARN_STREAM( "unable to contact the server " << SRV_??? << " - timeout expired (60s) " );
	// return ;
}
ROS_INFO_STREAM("["<<action_name<<"] "<<"client " << SRV_??? << " ... OK!");

 
// call the service 
if( !cl_???.call( ??? ) ) 
{ 
	ROS_WARN_STREAM( "unable to make a service request to " << SRV_??? << " -- failed calling service " 
		<< "[" << SRV_??? << "]"
		<< (!cl_???.exists( ) ? " -- it seems not opened" : "") );
	// return;
}

*/

namespace KCL_rosplan
{


// === BASE METHODS === //

// class constructor
RP_rcl_replan::RP_rcl_replan( ros::NodeHandle& nh_ ) : 
	RPActionInterface( ),
	nh( nh_ )
{
	// action feedback
	this->fb.action_name = ROSPLAN_ACTION_NAME;
	
	this->nh = nh_;
	
	ROS_INFO_STREAM("["<<action_name<<"] "<<"client " << SRV_REPLAN << " ...");
	cl_replan = nh.serviceClient<std_srvs::Empty>( SRV_REPLAN );
	if( !cl_replan.waitForExistence( ros::Duration( 60 ) ) )
	{
		ROS_WARN_STREAM( "unable to contact the server " << SRV_REPLAN << " - timeout expired (60s) " );
		return;
	}
	ROS_INFO_STREAM("["<<action_name<<"] "<<"client " << SRV_REPLAN << " ... OK!");
}


// class destructor
RP_rcl_replan::~RP_rcl_replan( )
{
	// ...
}




// === CONCRETE CALLBACK === //

// ...
bool RP_rcl_replan::concreteCallback( const rosplan_dispatch_msgs::ActionDispatch::ConstPtr& msg )
{
	// try to replan
	std_srvs::Empty sm;
	if( !cl_replan.call( sm ) ) 
	{ 
		ROS_WARN_STREAM( "unable to make a service request to " << SRV_REPLAN << " -- failed calling service " 
			<< "[" << SRV_REPLAN << "]"
			<< (!cl_replan.exists( ) ? " -- it seems not opened" : "") );
		
		this->fb.fb_failure( msg->parameters, "SERVICE ERROR: unable to contact '" + std::string(SRV_REPLAN) + "'" );
		
		return false;
	}
	
	return true;
}




// === PRIVATE METHODS === //

// ...


}
