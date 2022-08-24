
/********************************************//**
*  
* @file navigation_unit.cpp
* 
* @brief mission manager bridge between the robocluedo ROSPlan framework 
* 	and the navigation controller
* 
* This node is a bridge between the ROSPlan framework, which assumes 
* some other node implemented the service for the navigation from rosplan, 
* and the navigation system: a service call into a service call. 
* 
* The task of this node is quite simple: when received a request from the
* ROSPlan module, the node associates the cartesian coordinates of the
* pointed waypoint, then calls the navigation service; when the navigation
* has finished, the node adapt and returns the response to ROSPlan.
* 
* There are also other sub-tasks performed by this node: listening for the
* markers in order to find the coordinates of the waypoints, for instance,
* using a one-shot subscriber. Or also to initialise the navigation system. 
* 
* @note This implementation supports the develop mode: waitey, debug print
* 
* @note the node requires the move_base controller
* 
* @authors Francesco Ganci
* @version v1.1
* 
***********************************************/

#define NODE_NAME "navigation_unit"

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
#define WAITKEY { if( DEVELOP_WAITKEY ) { std::cout << "press ENTER to continue ... " ; std::cin.get( ) ; std::cout << "go!" << std::endl ; } }

#include "ros/ros.h"
#include "robocluedo_rosplan_msgs/NavigationCommand.h"
#include "robocluedo_movement_controller_msgs/NavigationService.h"
#include "robocluedo_movement_controller_msgs/Pose2D.h"
#include "robocluedo_movement_controller_msgs/Algorithm.h"
#include "visualization_msgs/MarkerArray.h"
#include "visualization_msgs/Marker.h"

#include <map>
#include <signal.h>
#include <math.h>

#include <string>
#define SS(chr) std::string( chr )
#define SSS(el) std::to_string( el )

#define SERVICE_ROSPLAN_NAVIGATION "/robocluedo/navigation_command"
#define SERVICE_SET_ALGORITHM "/navigation_manager/set_algorithm"
#define SERVICE_NAVIGATION "/navigation_manager/navigation"
#define TOPIC_MARMERS "/visualization_marker"

// i.e. use the bug_m motion planning algorithm
#define NAV_ALGO_BUG_M 0

// move base motion planning algorithm
#define NAV_ALGO_MOVE_BASE 1

#define SCALING_FACTOR 1.0

/********************************************//**
 *  
 * \brief implementation of the node
 * 
 * The navigation unit is implemented as a class containing all the services
 * and the topics needed to perform the navigation. 
 * 
 ***********************************************/
class node_navigation_unit
{
public:
	
	/********************************************//**
	 *  
	 * \brief node class constructor
	 * 
	 * here the node opens all the channels with the other components. 
	 * 
	 ***********************************************/
	node_navigation_unit( )
	{
		// topic markers
		WTLOG( "sub topic " << TOPIC_MARMERS << " ... " );
		sub_markers = nh.subscribe( TOPIC_MARMERS, 1, &node_navigation_unit::cbk_markers, this );
		WTLOG( "sub topic " << TOPIC_MARMERS << " ...  OK" );
		
		// service rosplan to unit
		WTLOG( "advertising service " << SERVICE_ROSPLAN_NAVIGATION << " ... " );
		srv_rosplan_nav = nh.advertiseService( SERVICE_ROSPLAN_NAVIGATION, &node_navigation_unit::cbk_navigation, this );
		WTLOG( "advertising service " << SERVICE_ROSPLAN_NAVIGATION << " ... OK" );
		
		// client navigation controller
		WTLOG( "opening client " << SERVICE_NAVIGATION << " ... " );
		cl_nav = nh.serviceClient<robocluedo_movement_controller_msgs::NavigationService>( SERVICE_NAVIGATION );
		if( !cl_nav.waitForExistence( ros::Duration(60) ) )
		{
			WTERR( "unable to contact the server " << SERVICE_NAVIGATION << " - timeout expired (60s) " );
			return;
		}
		WTLOG( "opening client " << SERVICE_NAVIGATION << " ... OK" );
		
		// client navigation algorithm
		WTLOG( "opening client " << SERVICE_SET_ALGORITHM << " ... " );
		cl_nav_algorithm = nh.serviceClient<robocluedo_movement_controller_msgs::Algorithm>( SERVICE_SET_ALGORITHM );
		if( !cl_nav_algorithm.waitForExistence( ros::Duration(60) ) )
		{
			WTERR( "unable to contact the server " << SERVICE_SET_ALGORITHM << " - timeout expired (60s) " );
			return;
		}
		WTLOG( "opening client " << SERVICE_SET_ALGORITHM << " ... OK" );
	}
	
	/** spin function: just wait for shutdown */
	void spin( )
	{
		// simple spin
		ros::waitForShutdown( );
	}
	
	/********************************************//**
	 *  
	 * \brief the service required by the rosplan package
	 * 
	 * A service call inside the service call: the node, after received the
	 * request, translates it into a request for the navigation manager, then
	 * calls that service and waits. When the navigation ends, the service 
	 * of this node returns a proper response to the ROSPlan framework. 
	 * 
	 * The service does also something else. First of all, it tries to
	 * set the algorithm the first time it is called. And second, it waits
	 * for the first reading of the markers from the markers publisher. These
	 * operations are performed only during the first service call. 
	 * 
	 * @param request the waypoint to reach
	 * @param response success or not?
	 * 
	 ***********************************************/
	bool cbk_navigation( 
		robocluedo_rosplan_msgs::NavigationCommand::Request& req, 
		robocluedo_rosplan_msgs::NavigationCommand::Response& res )
	{
		WTLOG( "received a request (wp=)" << req.waypoint );
		
		if( !mpl_enabled )
		{
			WTLOG( "enabling motion planning algorithm..." );
			
			robocluedo_movement_controller_msgs::Algorithm algo;
			// algo.request.algorithm = NAV_ALGO_BUG_M;
			algo.request.algorithm = NAV_ALGO_MOVE_BASE;
			algo.request.enabled = true;
			
			if( !cl_nav_algorithm.call( algo ) )
			{
				WTWARN( "unable to contact the service to enable the navigation algorithm." );
				
				res.success = false;
				return true;
			}
			
			if( !algo.response.success )
			{
				WTWARN( "unable to activate the navigation controller! details: " << algo.response.details );
				
				res.success = false;
				return true;
			}
			
			WTLOG( "enabling motion planning algorithm... OK" );
		}
		
		if( !found_markers )
		{
			WTLOG( "waiting for markers... " );
			ros::Rate r(5);
			do
				r.sleep();
			while( !found_markers );
			WTLOG( "found markers! " );
		}
		
		// before starting, check if the marker exists
		if( waypoints.find( req.waypoint ) == waypoints.end() )
		{
			WTWARN( "the marker " << req.waypoint << " does not exist." );
			
			res.success = false;
			return true;
		}
		
		// send the command
		WTLOG( "sending the navigation command..." );
		
		robocluedo_movement_controller_msgs::NavigationService cmd;
		cmd.request.target = waypoints[req.waypoint];
		WTLOG("waypoint with coordinates (" << cmd.request.target.x << ", " << cmd.request.target.y << ", " << cmd.request.target.yaw << ")");
		
		if( !cl_nav.call( cmd ) )
		{
			WTWARN( "unable to contact the navigation service" );
			
			res.success = false;
			return true;
		}
		
		// check for success
		if( !cmd.response.success )
			WTWARN( "navigation controlled FAILED -- explaination: " << cmd.response.details );
		
		WTLOG("done!");
		res.success = cmd.response.success;
		return true;
	}
	
	/********************************************//**
	 *  
	 * \brief "one-shot" listener for the markers from the Oracle
	 * 
	 * The callback stores the markers, then turns off the subscription to
	 * not waste computational resources in a continuous updating of the 
	 * markers.
	 * 
	 * @note we're assuming here that the markers don't change during
	 * the simulation. If the markers are able to move though, the 
	 * subscription should keep opened. 
	 * 
	 * @note currently, the function assumes that the index of a marker
	 * 	inside the array is its ID as well. for instance, data->markers[2]
	 * 	is called "wp3"  by the function. 
	 * 
	 ***********************************************/
	void cbk_markers( const visualization_msgs::MarkerArray::ConstPtr& data )
	{
		WTLOG( "reading markers ... " );
		
		// read the markers
		int i = 0;
		// float max_h = 0.f;
		for( const visualization_msgs::Marker& mrk : data->markers )
		{
			std::string marker_name = SS("wp") + SSS(i+1);
			
			robocluedo_movement_controller_msgs::Pose2D markerpose;
			
			markerpose.x = mrk.pose.position.x * SCALING_FACTOR;
			markerpose.y = mrk.pose.position.y * SCALING_FACTOR;
			
			markerpose.yaw = atan2( markerpose.y, markerpose.x ); // atan2 from -pi to pi
			if( markerpose.yaw > 3.135 )
				markerpose.yaw = 3.135; 
			else if( markerpose.yaw < -3.135 )
				markerpose.yaw = 3.135;
			
			waypoints[marker_name] = markerpose;
			
			++i;
			WTLOG( "received (" << i << ") marker with data (x=" << markerpose.x << ", y=" << markerpose.y << ", yaw= " << markerpose.yaw << ")" );
		}
		
		// even add the center
		{
			robocluedo_movement_controller_msgs::Pose2D markerpose;
			markerpose.x = 0.0;
			markerpose.y = 0.0;
			markerpose.yaw = 0.0;
			waypoints["center"] = markerpose;
		}
		
		// unsubscribe
		sub_markers.shutdown( );
		
		found_markers = true;
		WTLOG( "reading markers done -- found " << i << " markers" );
	}

private:
	
	/// ROS node handle
	ros::NodeHandle nh;
	
	/// service handle rosplan to unit
	ros::ServiceServer srv_rosplan_nav;
	
	/// client handle unit to navigation
	ros::ServiceClient cl_nav;
	
	/// subscription handle to markers from the Oracle
	ros::Subscriber sub_markers;
	
	/// to change the algorithm (in case...)
	ros::ServiceClient cl_nav_algorithm;
	
	/// positions of the waypoints
	std::map<std::string, robocluedo_movement_controller_msgs::Pose2D> waypoints;
	
	/// signal fro the markers listener
	bool found_markers = false;
	
	/// is the motion planning algorithm active?
	bool mpl_enabled = false;
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
	
	ros::AsyncSpinner spinner( 4 );
	spinner.start( );
	
	WTLOG( "starting ... " );
	node_navigation_unit node;
	
	WTLOG( "ready" );
	node.spin( );
	
	return 0;
}



