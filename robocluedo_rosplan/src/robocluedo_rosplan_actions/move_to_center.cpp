
/********************************************//**
*  
* @file move_to_center.cpp
* @brief ROSPlan action implementation
* 
* @authors Francesco Ganci
* @version v1.0
* 
* @see feedback_manager.h
* 
***********************************************/

#include "robocluedo_rosplan_actions/move_to_center.h"

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
RP_rcl_move_to_center::RP_rcl_move_to_center( ros::NodeHandle& nh_ ) : 
	RPActionInterface( ),
	nh( nh_ )
{
	// action feedback
	this->fb.action_name = ROSPLAN_ACTION_NAME;
	this->nh = nh_;
	
	ROS_INFO_STREAM("["<<action_name<<"] "<<"client " << SRV_NAVIGATION << " ...");
	cl_navigation = nh.serviceClient<robocluedo_rosplan_msgs::NavigationCommand>( SRV_NAVIGATION );
	if( !cl_navigation.waitForExistence( ros::Duration( 60 ) ) )
	{
		ROS_WARN_STREAM("["<<action_name<<"] "<<"unable to contact the server " << SRV_NAVIGATION << " - timeout expired (60s) " );
		
		return;
	}
	ROS_INFO_STREAM("["<<action_name<<"] "<<"client " << SRV_NAVIGATION << " ... OK!");
}


// class destructor
RP_rcl_move_to_center::~RP_rcl_move_to_center( )
{
	// ...
}




// === CONCRETE CALLBACK === //

// implementation of (move-to ?from ?to)
bool RP_rcl_move_to_center::concreteCallback( const rosplan_dispatch_msgs::ActionDispatch::ConstPtr& msg )
{
	std::string from = msg->parameters[0].value;
	// std::string to = msg->parameters[1].value;
	ROS_INFO_STREAM("["<<action_name<<"] "<<"(move-to-center ?" << from << ")");
	
	robocluedo_rosplan_msgs::NavigationCommand nav_srv;
	nav_srv.request.waypoint = "center";
	
	// call the service 
	if( !cl_navigation.call( nav_srv ) ) 
	{ 
		ROS_WARN_STREAM("["<<action_name<<"] "<<"unable to make a service request to " << SRV_NAVIGATION << " -- failed calling service " 
			<< "[" << SRV_NAVIGATION << "]"
			<< (!cl_navigation.exists( ) ? " -- it seems not opened" : "") );
			
		this->fb.fb_failure( msg->parameters, "SERVICE ERROR: unable to contact '" + std::string(SRV_NAVIGATION) + "'" );
			
		return false;
	}
	
	if( nav_srv.response.success )
		return true;
	else
	{
		this->fb.fb_hw_navigation_failure( msg->parameters, "navigation failed (success flag not true)" );
		return false;
	}
	
	return true;
}




// === PRIVATE METHODS === //

// ...


}
