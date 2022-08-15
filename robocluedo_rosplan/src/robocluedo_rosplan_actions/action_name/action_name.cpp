
/********************************************//**
*  
* @file ???.cpp
* @brief ROSPlan action implementation
* 
* @authors Francesco Ganci
* @version v1.0
* 
* @see feedback_manager.h
* 
***********************************************/

#include "robocluedo_rosplan_actions/action_name.h"

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
	ROS_WARN_STREAM("["<<action_name<<"] "<<"unable to contact the server " << SRV_??? << " - timeout expired (60s) " );
	// return ;
}
ROS_INFO_STREAM("["<<action_name<<"] "<<"client " << SRV_??? << " ... OK!");

 
// call the service 
if( !cl_???.call( ??? ) ) 
{ 
	ROS_WARN_STREAM("["<<action_name<<"] "<<"unable to make a service request to " << SRV_??? << " -- failed calling service " 
		<< "[" << SRV_??? << "]"
		<< (!cl_???.exists( ) ? " -- it seems not opened" : "") );
	// return;
}

*/

namespace KCL_rosplan
{


// === BASE METHODS === //

// class constructor
RP_rcl_action_name::RP_rcl_action_name( ros::NodeHandle& nh_ ) : 
	RPActionInterface( ),
	nh( nh_ )
{
	// action feedback
	this->fb.action_name = ROSPLAN_ACTION_NAME;
	
	this->nh = nh_;
	
	// ...
}


// class destructor
RP_rcl_action_name::~RP_rcl_action_name( )
{
	// ...
}




// === CONCRETE CALLBACK === //

// ...
bool RP_rcl_action_name::concreteCallback( const rosplan_dispatch_msgs::ActionDispatch::ConstPtr& msg )
{
	// ...
	
	return true;
}




// === PRIVATE METHODS === //

// ...


}
