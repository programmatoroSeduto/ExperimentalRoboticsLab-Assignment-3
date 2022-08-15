
/********************************************//**
*  
* @file collect_hint.cpp
* @brief ROSPlan action implementation
* 
* @authors Francesco Ganci
* @version v1.0
* 
* @see feedback_manager.h
* 
***********************************************/

#include "robocluedo_rosplan_actions/collect_hint.h"

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
RP_rcl_collect_hint::RP_rcl_collect_hint( ros::NodeHandle& nh_ ) : 
	RPActionInterface( ),
	nh( nh_ )
{
	// action feedback
	this->fb.action_name = ROSPLAN_ACTION_NAME;
	this->nh = nh_;
	
	// require the manipulation client
	ROS_INFO_STREAM("["<<action_name<<"] "<<"client " << SRV_MANIPULATION << " ...");
	cl_manip = nh.serviceClient<robocluedo_rosplan_msgs::ManipulationCommand>( SRV_MANIPULATION );
	if( !cl_manip.waitForExistence( ros::Duration( 60 ) ) )
	{
		ROS_WARN_STREAM("["<<action_name<<"] "<<"unable to contact the server " << SRV_MANIPULATION << " - timeout expired (60s) " );
		
		return;
	}
	ROS_INFO_STREAM("["<<action_name<<"] "<<"client " << SRV_MANIPULATION << " ... OK!");
}


// class destructor
RP_rcl_collect_hint::~RP_rcl_collect_hint( )
{
	// ...
}




// === CONCRETE CALLBACK === //

// ...
bool RP_rcl_collect_hint::concreteCallback( const rosplan_dispatch_msgs::ActionDispatch::ConstPtr& msg )
{
	std::string wp = msg->parameters[0].value;
	ROS_INFO_STREAM("["<<action_name<<"] "<<"(collect-hint ?" << wp << ")" );
	
	{
	robocluedo_rosplan_msgs::ManipulationCommand cmd;
	cmd.request.waypoint = wp;
	
	// manipulator near the marker
	ROS_INFO_STREAM("["<<action_name<<"] "<<"manipulator PHASE 1");
	cmd.request.home_position = false;
	if( !cl_manip.call( cmd ) ) 
	{ 
		ROS_WARN_STREAM("["<<action_name<<"] "<<"unable to make a service request to " << SRV_MANIPULATION << " -- failed calling service " 
			<< "[" << SRV_MANIPULATION << "]"
			<< (!cl_manip.exists( ) ? " -- it seems not opened" : "") );
		
		this->fb.fb_failure( msg->parameters, "SERVICE ERROR: unable to contact '" + std::string(SRV_MANIPULATION) + "' (manipulation phase 1)" );
		
		return false;
	}
	
	// chech the outcome of the action
	if( !cmd.response.success )
	{
		ROS_WARN_STREAM("["<<action_name<<"] WARNING: "<<"manipulation didn't succeeded (movement phase FAILED)");
		
		this->fb.fb_hw_manipulation_failure( msg->parameters, "movement PHASE 1 failed (success flag is false)" );
		
		return false;
	}
	}
	
	
	// wait
	ROS_INFO_STREAM("["<<action_name<<"] "<<"manipulator WAITING");
	(ros::Duration(2)).sleep();
	
	
	{
	robocluedo_rosplan_msgs::ManipulationCommand cmd;
	cmd.request.waypoint = wp;
	
	// manipulator at home position
	ROS_INFO_STREAM("["<<action_name<<"] "<<"manipulator PHASE 2");
	cmd.request.home_position = true;
	if( !cl_manip.call( cmd ) ) 
	{ 
		ROS_WARN_STREAM("["<<action_name<<"] "<<"unable to make a service request to " << SRV_MANIPULATION << " -- failed calling service " 
			<< "[" << SRV_MANIPULATION << "]"
			<< (!cl_manip.exists( ) ? " -- it seems not opened" : "") );
		
		this->fb.fb_failure( msg->parameters, "SERVICE ERROR: unable to contact '" + std::string(SRV_MANIPULATION) + "' (manipulation phase 1)" );
		
		return false;
	}
	
	// chech the outcome of the action
	if( !cmd.response.success )
	{
		ROS_WARN_STREAM("["<<action_name<<"] WARNING: "<<"manipulation didn't succeeded (movement phase FAILED)");
		
		this->fb.fb_hw_manipulation_failure( msg->parameters, "movement PHASE 2 failed (success flag is false)" );
		
		return false;
	}
	}
	
	return true;
}




// === PRIVATE METHODS === //

// ...


}
