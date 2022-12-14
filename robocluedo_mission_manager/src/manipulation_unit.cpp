
/********************************************//**
*  
* @file manipulation_unit.cpp
* 
* @brief manipulator high-level motion planning
* 
* This is node has been ment, in the RCL#2 project, to act as a communication
* mean between the ROSPlan framework and the real manipulation controller, 
* but since the RCL#3 project doesn't require (so far) a explicit manipulation
* capability from ROSPlan, the node has here another function: managing the 
* movement of the arm *during the exploration of the environment*.
* 
* To be more precise, this new version of the manipulation uit, when 
* turned on, makes the robotic manipulator to assume a pose randomly
* choosen while the robot is moving. This enables the robot to get also
* hints that, with a more deterministic movement of the camera, could be
* missed since the robot acts always in the same way. There are some
* unluky cases, but most of times it seems to work fine. 
* 
* @note This implementation supports the develop mode: waitey, debug print
* 
* @authors Francesco Ganci
* @version v1.0 
* 
***********************************************/

#define NODE_NAME "manipualtion_unit"

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
#include "robocluedo_rosplan_msgs/ManipulationCommand.h"
#include "robocluedo_movement_controller_msgs/ManipulatorPosition.h"
#include "robocluedo_movement_controller_msgs/ManipulatorPositionAsync.h"
#include "robocluedo_movement_controller_msgs/Pose2D.h"
#include "visualization_msgs/MarkerArray.h"
#include "visualization_msgs/Marker.h"
#include "std_srvs/SetBool.h"

#include <map>
#include <signal.h>
#include <math.h>

#include <string>
#define SS(chr) std::string( chr )
#define SSS(el) std::to_string( el )

// rosplan manipulation command
#define SERVICE_ROBOPLAN_MANIP "/robocluedo/manipulator_command"

// manipulation controller service
#define SERVICE_MANIP "/tip_pos"

#define TOPIC_MARMERS "/visualization_marker"
#define TOPIC_MANIP_ASYNC "/tip_pos_async"

#define SERVICE_RANDOM_ARM_SWITCH "/tip_pos_auto_switch"

#define POSE_HOME 0
#define POSE_CAM_LEFT 1
#define POSE_CAM_RIGHT 2
#define POSE_CAM_BACK 3
#define POSE_CAM_LOW 4
#define POSE_CAM_LOW_LEFT 5
#define POSE_CAM_LOW_BACK 6
#define POSE_CAM_LOW_RIGHT 7
#define POSE_HOME_LOW 8

/********************************************//**
 *  
 * \class nove_manipulation_unit
 * 
 * \brief class implementation of the node
 * 
 * @note in future, someone maybe wants to add functionalities to ROSPlan
 * robocluedo interface: that's why this bridge is located in its node, and
 * not in only one node. 
 * 
 ***********************************************/
class node_manipulation_unit
{
public:
	
	/// node constructor
	node_manipulation_unit( )
	{
		// manipulation controller
		WTLOG( "Opening client " << LOGSQUARE( SERVICE_MANIP ) << "..." );
		cl_manip = nh.serviceClient<robocluedo_movement_controller_msgs::ManipulatorPosition>( SERVICE_MANIP );
		if( !cl_manip.waitForExistence( ros::Duration( 60 ) ) )
		{
			WTERR( "unable to contact the server " << SERVICE_MANIP << " - timeout expired (60s) " );
			return;
		}
		WTLOG( "Opening client " << LOGSQUARE( SERVICE_MANIP ) << "... OK" );
		
		WTLOG( "Advertising service " << LOGSQUARE( SERVICE_RANDOM_ARM_SWITCH ) << "..." );
		srv_auto_manip_switch = nh.advertiseService( SERVICE_RANDOM_ARM_SWITCH, &node_manipulation_unit::cbk_auto_manip_switch, this );
		WTLOG( "Advertising service " << LOGSQUARE( SERVICE_RANDOM_ARM_SWITCH ) << "... OK" );
		
		// publisher
		WTLOG( "publisher " << TOPIC_MANIP_ASYNC << " ... " );
		pub_motion_async = nh.advertise<robocluedo_movement_controller_msgs::ManipulatorPositionAsync>( TOPIC_MANIP_ASYNC, 3 );
		WTLOG( "publisher " << TOPIC_MANIP_ASYNC << " ... OK" );
	}
	
	/********************************************//**
	 *  
	 * \brief spin function: send a new pose each 5 seconds
	 * 
	 * When activated, the unit sends a new pose, randomly taken from 
	 * the set of the valid ones, to the manipulation controller, and
	 * makes it running asynchronosly. 
	 * 
	 * @note currently the manipulation controller in asynchronous mode
	 * doesn't implement a check for the progress, so the node sends a new
	 * position with a regular time interval which is assumed to be large
	 * enough to complete the transition from one position to another. 
	 * 
	 ***********************************************/
	void spin( )
	{
		robocluedo_movement_controller_msgs::ManipulatorPositionAsync msg;
		
		ros::Duration d( 5 );
		while( ros::ok( ) )
		{
			if( auto_manip_active )
			{
				msg.command = rand( ) % 9;
				msg.enabled = true;
				
				WTLOG( "new pose: " << msg.command );
				
				pub_motion_async.publish( msg );
			}
			else
			{
				msg.command = 0;
				msg.enabled = false;
				
				pub_motion_async.publish( msg );
			}
			
			d.sleep( );
		}
	}
	
	/********************************************//**
	 *  
	 * \brief switch the automatic manipulation
	 * 
	 * the service allows to switch on and off the random automatic 
	 * movements of the arm. 
	 * 
	 ***********************************************/
	bool cbk_auto_manip_switch( 
		std_srvs::SetBool::Request& req, std_srvs::SetBool::Response& res )
	{
		auto_manip_active = req.data;
		
		if( req.data )
		{
			WTLOG( "auto manipulation ENABLED" );
		}
		else
		{
			WTLOG( "auto manipulation DISABLED" );
		}
		
		res.success = true;
		return true;
	}
	
private:
	
	/// ROS node handle
    ros::NodeHandle nh;
    
    /// manipulation controller client
    ros::ServiceClient cl_manip;
    
    /// subscription for the async motion
    ros::Publisher pub_motion_async;
    
    /// switch for the authomatic manipulation
    ros::ServiceServer srv_auto_manip_switch;
    
    /// automatic manipulation on? (default: not active)
    bool auto_manip_active = false;
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
	
	ros::AsyncSpinner spinner( 1 );
	spinner.start( );
	
	WTLOG( "starting ... " );
	node_manipulation_unit node;
	
	WTLOG( "ready" );
	node.spin( );
	
	return 0;
}
