
#define NODE_NAME "manipulation_controller"

/********************************************//**
*  
* @file manipulation_controller.cpp
* @brief controller for performig the manipulation
* 
* In a nutshell, this node implements a very simple service able to 
* communicate with moveit in order to perform a manipulation with
* the RoboCLuedo hunter manipulator. The robot comes with a set of
* predefined poses, that the controller can call under request. 
* 
* This version of the node, except for containing new poses referred
* to the new model of robot, introduces a very raw implementation of
* a asynchronous movement, performed using a simple topic. 
* 
* @authors Francesco Ganci
* @version v1.2
* 
* @note this implementation supports the developer mode: waitkey, print develop.
* 
* @todo implement the asynchronous movement using a ROS action instead. Or,
* 	at least, introduce a signal. 
* 
* @todo introduce cartesian planning and a clear end effector
* 
* @todo the way the node uses moveit is a bit raw ... it would deserve a 
* 	review. 
* 
***********************************************/

#define LOGSQUARE( str )  "[" << str << "] "
#define OUTLABEL          LOGSQUARE( NODE_NAME )
#define TLOG( msg )       ROS_INFO_STREAM( OUTLABEL << msg )
#define TWARN( msg )      ROS_WARN_STREAM( OUTLABEL << "WARNING: " << msg )
#define TERR( msg )       ROS_WARN_STREAM( OUTLABEL << "ERROR: " << msg )

#define DEVELOP_MODE false

#define DEVELOP_PRINT false
#define WTLOG( msg )  { if( DEVELOP_PRINT ) { ROS_INFO_STREAM( OUTLABEL << msg ); } }
#define WTWARN( msg ) { if( DEVELOP_PRINT ) { ROS_WARN_STREAM( OUTLABEL << msg ); } }
#define WTERR( msg )  { if( DEVELOP_PRINT ) { ROS_WARN_STREAM( OUTLABEL << msg ); } }

#define DEVELOP_WAIKEY false
#define WAITKEY { if( WAITKEY_ENABLED ) { std::cout << "press ENTER to continue ... " ; std::cin.get( ) ; std::cout << "go!" << std::endl ; } }

#include "ros/ros.h"
#include "moveit/move_group_interface/move_group_interface.h"
#include "moveit/planning_scene_interface/planning_scene_interface.h"
#include "robocluedo_movement_controller_msgs/ManipulatorPosition.h"
#include "robocluedo_movement_controller_msgs/ManipulatorPositionAsync.h"
//include <moveit_visual_tools/moveit_visual_tools.h>
//include <moveit_msgs/DisplayRobotState.h>
//include <moveit_msgs/DisplayTrajectory.h>
//include <moveit_msgs/AttachedCollisionObject.h>
//include <moveit_msgs/CollisionObject.h>

#include "signal.h"

#include <string>
#define SS(chr) std::string( chr )
#define SSS(el) std::to_string( el )

#include <mutex>

#define ARM_PLANNING_GROUP "arm_group"

#define SERVICE_MANIP "/tip_pos"
#define TOPIC_MANIP_ASYNC "/tip_pos_async"

#define POSE_SET_CARTESIAN -1

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

/** implementation of the node */
class node_manipulation_controller
{
public:
	
	/********************************************//**
	 *  
	 * \brief class constructor
	 * 
	 * the class constructor performs the initialisation of the
	 * move group interface, then sets up some parameters, and finally 
	 * mves the arm in the home position.
	 * 
	 * When launched, the node first of all puts the robotic manipulator
	 * in a known state, which is the home position. 
	 * 
	 ***********************************************/
	node_manipulation_controller(  ) : mgi( ARM_PLANNING_GROUP )
	{
		WTLOG( "Advertising service " << LOGSQUARE( SERVICE_MANIP ) << "..." );
		srv_manip = nh.advertiseService( SERVICE_MANIP, &node_manipulation_controller::cbk_manip, this );
		WTLOG( "Advertising service " << LOGSQUARE( SERVICE_MANIP ) << "... OK" );
		
		WTLOG( "Subscription to topic " << TOPIC_MANIP_ASYNC << " ... " );
		sub_moveit_async = nh.subscribe( TOPIC_MANIP_ASYNC, 3, &node_manipulation_controller::cbk_manip_async, this );
		WTLOG( "Subscription to topic " << TOPIC_MANIP_ASYNC << " ... OK" );
		
		(ros::Duration(1.0)).sleep( );
		
		// MoveIt settings
		// mgi.setPlanningTime(10.0);
		
		if( !mgi.setEndEffector( "cluedo_ee" ) )
		{	WTLOG( "unable to set the end effector 'cluedo_ee'" ); }
		else
		{
			mgi.setGoalJointTolerance(0.01);
			mgi.setGoalPositionTolerance(0.01);
			mgi.setGoalOrientationTolerance(0.01);
		}
		
		mgi.setPoseReferenceFrame("base_link");
		WTLOG("Planning frame: " << mgi.getPlanningFrame().c_str());
		
		mgi.setNumPlanningAttempts(10);
		mgi.setPlanningTime(10.0);
		mgi.allowReplanning(true);
		
		// set the initial position of the arm
		mgi.setNamedTarget( POSE_NAME_HOME );
		mgi.move( );
	}
	
	/// simple spin (wait for shutdown)
	void spin( )
	{
		ros::waitForShutdown( );
	}
	
	/********************************************//**
	 *  
	 * \brief sybchronous manipulation
	 * 
	 * given a poture, the service sets it and moves the robotic manipulator
	 * until it hasn't achieved that. 
	 * 
	 * @param req the posture to be set
	 * @param res if the motion planning succeeded or not
	 * 
	 ***********************************************/
	bool cbk_manip( 
		robocluedo_movement_controller_msgs::ManipulatorPosition::Request& req, 
		robocluedo_movement_controller_msgs::ManipulatorPosition::Response& res )
	{
		std::string cmd = "";
		
		switch(req.command)
		{
		/*
		case POSE_SET_CARTESIAN:
		{
			// set cartesian pose from the message
			geometry_msgs::Pose tg;
			tg.position = req.ee_target;
			tg.orientation.x = 0.0;
			tg.orientation.y = 0.0;
			tg.orientation.z = 0.0;
			tg.orientation.w = 1.0;
			
			mgi.setPoseTarget( tg );
		}
		break;
		*/
		case POSE_HOME:
		{
			cmd = POSE_NAME_HOME;
			mgi.setNamedTarget( cmd );
		}
		break;
		case POSE_CAM_LEFT:
		{
			cmd = POSE_NAME_CAM_LEFT;
			mgi.setNamedTarget( cmd );
		}
		break;
		case POSE_CAM_RIGHT:
		{
			cmd = POSE_NAME_CAM_RIGHT;
			mgi.setNamedTarget( cmd );
		}
		break;
		case POSE_CAM_BACK:
		{
			cmd = POSE_NAME_CAM_BACK;
			mgi.setNamedTarget( cmd );
		}
		break;
		case POSE_CAM_LOW:
		{
			cmd = POSE_NAME_CAM_LOW;
			mgi.setNamedTarget( cmd );
		}
		break;
		case POSE_CAM_LOW_LEFT:
		{
			cmd = POSE_NAME_CAM_LOW_LEFT;
			mgi.setNamedTarget( cmd );
		}
		break;
		case POSE_CAM_LOW_BACK:
		{
			cmd = POSE_NAME_CAM_LOW_BACK;
			mgi.setNamedTarget( cmd );
		}
		break;
		case POSE_CAM_LOW_RIGHT:
		{
			cmd = POSE_NAME_CAM_LOW_RIGHT;
			mgi.setNamedTarget( cmd );
		}
		break;
		case POSE_HOME_LOW:
		{
			cmd = POSE_NAME_HOME_LOW;
			mgi.setNamedTarget( cmd );
		}
		break;
		default:
		{
			res.success = false;
			res.details = SS("pose code unknown (received POSE=") + SSS(req.command) + SS(")");
			WTLOG( "" << res.details );
			
			return true;
		}
		break;
		}
		
		// motion plan
		mtx.lock();
		if( mgi.plan( plan ) != moveit::planning_interface::MoveItErrorCode::SUCCESS )
		{
			mtx.unlock();
			res.success = false;
			res.details = "motion planning phase failed -- unable to make a plan";
			WTLOG( "" << res.details );
			
			return true;
		}
		
		// plan execution
		mgi.execute( plan );
		mtx.unlock();
		
		res.success = true;
		res.details = "";
		
		return true;
	}
	
	/********************************************//**
	 *  
	 * \brief async manpulation command (from topic)
	 * 
	 * @note once activated, there's no way to stop the motion!
	 * 
	 * @todo implement a progress checking
	 * 
	 ***********************************************/
	void cbk_manip_async( const robocluedo_movement_controller_msgs::ManipulatorPositionAsync::ConstPtr& req )
	{
		movement_async_enabled = req->enabled;
		
		if( movement_async_enabled )
		{
			robocluedo_movement_controller_msgs::ManipulatorPosition mt;
			mt.request.command = req->command;
			
			cbk_manip( mt.request, mt.response );
		}
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
    
    /// subscriber moveit async
    ros::Subscriber sub_moveit_async;
    
    /// movement async enabled
    bool movement_async_enabled = true;
    
    /// a mutex for controlling the service calling and the async motion request
    std::mutex mtx;

};


void shut_msg( int sig )
{
	WTLOG( "stopping... " );
	ros::shutdown( );
}


int main( int argc, char* argv[] )
{
	ros::init( argc, argv, NODE_NAME, ros::init_options::NoSigintHandler );
	signal( SIGINT, shut_msg );
	ros::NodeHandle nh;
	
	// required by moveIt: run the node as AsyncSpinner
	ros::AsyncSpinner spinner( 3 );
	spinner.start( );
	
	WTLOG( "starting ... " );
	
	node_manipulation_controller node;
	
	WTLOG( "ready" );
	
	node.spin( );
	
	return 0;
}
