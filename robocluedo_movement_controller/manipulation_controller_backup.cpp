
#define NODE_NAME "manipulation_controller"

#define LOGSQUARE( str )  "[" << str << "] "
#define OUTLABEL          LOGSQUARE( NODE_NAME )
#define TLOG( msg )       ROS_INFO_STREAM( OUTLABEL << msg )
#define TWARN( msg )      ROS_WARN_STREAM( OUTLABEL << "WARNING: " << msg )
#define TERR( msg )       ROS_WARN_STREAM( OUTLABEL << "ERROR: " << msg )

#include "ros/ros.h"
#include "moveit/move_group_interface/move_group_interface.h"
#include "moveit/planning_scene_interface/planning_scene_interface.h"
#include "robocluedo_movement_controller_msgs/ManipulatorPosition.h"
//include <moveit_visual_tools/moveit_visual_tools.h>
//include <moveit_msgs/DisplayRobotState.h>
//include <moveit_msgs/DisplayTrajectory.h>
//include <moveit_msgs/AttachedCollisionObject.h>
//include <moveit_msgs/CollisionObject.h>

#include "signal.h"

#include <string>
#define SS(chr) std::string( chr )
#define SSS(el) std::to_string( el )

#define ARM_PLANNING_GROUP "arm_group"

#define SERVICE_MANIP "/tip_pos"

#define POSE_HOME 0
#define POSE_CAM_LEFT 1
#define POSE_CAM_RIGHT 2
#define POSE_CAM_BACK 3
#define POSE_CAM_LOW 4
#define POSE_CAM_LOW_LEFT 5
#define POSE_CAM_LOW_BACK 6
#define POSE_CAM_LOW_RIGHT 7
#define POSE_HOME_LOW 8

#define POSE_NAME_HOME "home"
#define POSE_NAME_CAM_LEFT "cam_left"
#define POSE_NAME_CAM_RIGHT "cam_right"
#define POSE_NAME_CAM_BACK "cam_back"
#define POSE_NAME_CAM_LOW "cam_low"
#define POSE_NAME_CAM_LOW_LEFT "cam_low_left"
#define POSE_NAME_CAM_LOW_BACK "cam_low_back"
#define POSE_NAME_CAM_LOW_RIGHT "cam_low_right"
#define POSE_NAME_HOME_LOW "home_low"


class node_manipulation_controller
{
public:
	
	node_manipulation_controller(  ) : mgi( ARM_PLANNING_GROUP )
	{
		TLOG( "Advertising service " << LOGSQUARE( SERVICE_MANIP ) << "..." );
		srv_manip = nh.advertiseService( SERVICE_MANIP, &node_manipulation_controller::cbk_manip, this );
		TLOG( "Advertising service " << LOGSQUARE( SERVICE_MANIP ) << "... OK" );
		
		(ros::Duration(1.0)).sleep( );
		
		// MoveIt settings
		mgi.setPlanningTime(10.0);
		
		// moveit infos
		TLOG("Planning frame: " << mgi.getPlanningFrame().c_str());
		
		// set the initial position of the arm
		mgi.setNamedTarget( POSE_NAME_HOME );
		mgi.move( );
	}
	
	void spin( )
	{
		ros::waitForShutdown( );
	}
	
	bool cbk_manip( 
		robocluedo_movement_controller_msgs::ManipulatorPosition::Request& req, 
		robocluedo_movement_controller_msgs::ManipulatorPosition::Response& res )
	{
		std::string cmd = "";
		
		switch(req.command)
		{
		case POSE_HOME:
		{
			cmd = POSE_NAME_HOME;
		}
		break;
		case POSE_CAM_LEFT:
		{
			cmd = POSE_NAME_CAM_LEFT;
		}
		break;
		case POSE_CAM_RIGHT:
		{
			cmd = POSE_NAME_CAM_RIGHT;
		}
		break;
		case POSE_CAM_BACK:
		{
			cmd = POSE_NAME_CAM_BACK;
		}
		break;
		case POSE_CAM_LOW:
		{
			cmd = POSE_NAME_CAM_LOW;
		}
		break;
		case POSE_CAM_LOW_LEFT:
		{
			cmd = POSE_NAME_CAM_LOW_LEFT;
		}
		break;
		case POSE_CAM_LOW_BACK:
		{
			cmd = POSE_NAME_CAM_LOW_BACK;
		}
		break;
		case POSE_CAM_LOW_RIGHT:
		{
			cmd = POSE_NAME_CAM_LOW_RIGHT;
		}
		break;
		case POSE_HOME_LOW:
		{
			cmd = POSE_NAME_HOME_LOW;
		}
		break;
		default:
		{
			res.success = false;
			res.details = SS("pose code unknown (received POSE=") + SSS(req.command) + SS(")");
			TLOG( "" << res.details );
			
			return true;
		}
		break;
		}
		mgi.setNamedTarget( cmd );
		
		// motion plan
		if( mgi.plan( plan ) != moveit::planning_interface::MoveItErrorCode::SUCCESS )
		{
			res.success = false;
			res.details = "motion planning phase failed -- unable to make a plan";
			TLOG( "" << res.details );
			
			return true;
		}
		
		// plan execution
		mgi.execute( plan );
		
		res.success = true;
		res.details = "";
		
		return true;
	}
	
private:
	
	/// ROS node handle
    ros::NodeHandle nh;
	
	/// moveIt group name
	std::string planning_group;
	
	/// planning interface
	moveit::planning_interface::MoveGroupInterface mgi;
	
	/// previously generated plan
	moveit::planning_interface::MoveGroupInterface::Plan plan;
    
    /// moveit service
    ros::ServiceServer srv_manip;

};


void shut_msg( int sig )
{
	TLOG( "stopping... " );
	ros::shutdown( );
}


int main( int argc, char* argv[] )
{
	ros::init( argc, argv, NODE_NAME, ros::init_options::NoSigintHandler );
	signal( SIGINT, shut_msg );
	ros::NodeHandle nh;
	
	// required by moveIt: run the node as AsyncSpinner
	ros::AsyncSpinner spinner( 2 );
	spinner.start( );
	
	TLOG( "starting ... " );
	
	node_manipulation_controller node;
	
	TLOG( "ready" );
	
	node.spin( );
	
	return 0;
}