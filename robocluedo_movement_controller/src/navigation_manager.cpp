
#define NODE_NAME "navigation_manager"

/********************************************//**
*  
* @file navigation_manager.cpp
* @brief interface with the navigation system.
* 
* the project could have may navigation algorihms implemented, in order 
* to make the robot behaviour more adaptable to the current situation.
* hence, the task of the navigation mannager is to provide an abstraction, 
* hiding the detals of many algorithms and allowing the program to choose
* one of the algorithms at runtine. 
* 
* @authors Francesco Ganci
* @version v2.0
* 
* @note this implementation supports the developer mode: waitkey, print develop.
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
#include "robocluedo_movement_controller_msgs/Algorithm.h"
#include "robocluedo_movement_controller_msgs/NavigationService.h"
#include "std_srvs/Empty.h"
#include "std_srvs/SetBool.h"

#include <map>
#include <signal.h>

#include <string>
#define SS(chr) std::string( chr )
#define SSS(el) std::to_string( el )

#define SERVICE_SET_ALGORITHM "/navigation_manager/set_algorithm"
#define SERVICE_NAVIGATION "/navigation_manager/navigation"
#define SERVICE_BUGM_SWITCH "/bug_switch"
#define SERVICE_BUGM_SIGNAL "/bug_m_signal"
#define SERVICE_MOVE_BASE_SWITCH "/nav_stack_switch"
#define SERVICE_MOVE_BASE_SIGNAL "/nav_stack_signal"



/********************************************//**
 *  
 * \brief virtual interface for a navigation implementation 
 * 
 * @note this class is also a template for a motion planning algorithm
 * 
 ***********************************************/
class navigation_controller
{
public:
	
	/// navigation identfier
	int nav_code = -1;
	
	/// navigation algorithm name
	std::string nav_name = "";
	
	/** class constructor */
	navigation_controller( std::string name = "", int code = -1 ) :
		nav_code( code )
	{ 
		if( name != "" )
			this->nav_name = name;
	}
	
	/** class destructor */
	~navigation_controller( ) { }
	
	/** (virtual) turn on the algorithm */
	virtual bool enable( )
	{
		this->enabled = true;
		return true;
	}
	
	/** (virtual) use the algorithm for the 2D navigation */
	virtual bool perform_navigation( 
		robocluedo_movement_controller_msgs::NavigationService::Request& req,
		robocluedo_movement_controller_msgs::NavigationService::Response& res )
	{
		return this->enabled;
	};
	
	/** (virtual) turn off the algorithm */
	virtual bool disable( )
	{
		this->enabled = false;
		return true;
	}
	
	/** check wether the algorithm is enabled or not */
	bool is_enabled( ) { return this->enabled; }

protected:
	
	/// ROS Node handle 
	ros::NodeHandle nh;
	
	/// activity state of the algorithm
	bool enabled = false;
};



/********************************************//**
 *  
 * \brief nav service for the low level bug_m algorithm
 * 
 * This controller uses the bug_m implementation to navigate to the target
 * and achieve whatever final orientation pose. 
 * 
 * This low level is a bit limited in many aspects:
 * - it cannot check if the target is reachable
 * - the algorithm doesn't work in complex environments with obstacles
 * 
 * The implementation turns on the channels at the first enable call, and
 * leaves them active for the entire lifecycle of the navigation manager. 
 * 
 ***********************************************/
class nav_bug_m : public navigation_controller
{
public:
	
	/** class constructor */
	nav_bug_m( std::string name = "", int code = -1 ) :
		navigation_controller( name, code )
	{ 
		// ...
	}
	
	/** class destructor */
	~nav_bug_m( )
	{ 
		// ...
	}
	
	/** turn on the algorithm */
	bool enable( )
	{
		std_srvs::SetBool cmd;
		cmd.request.data = false;
		
		if( !channels_enabled )
		{
			// open the client /bug_switch : std_srvs/SetBool
			WTLOG( "Opening client " << LOGSQUARE( SERVICE_BUGM_SWITCH ) << "..." );
			cl_bug_switch = nh.serviceClient<std_srvs::SetBool>( SERVICE_BUGM_SWITCH );
			if( !cl_bug_switch.waitForExistence( ros::Duration( 60 ) ) )
			{
				WTERR( "unable to contact the server " << SERVICE_BUGM_SWITCH << " - timeout expired (60s) " );
				return false;
			}
			WTLOG( "Opening client " << LOGSQUARE( SERVICE_BUGM_SWITCH ) << "... OK" );
			
			// open the service /bug_m_signal : std_srvs/Empty
			WTLOG( "advertising service " << LOGSQUARE( SERVICE_BUGM_SIGNAL ) << "..." );
			cl_bug_signal = nh.advertiseService( SERVICE_BUGM_SIGNAL, &nav_bug_m::cbk_bug_signal, this );
			WTLOG( "advertising service " << LOGSQUARE( SERVICE_BUGM_SIGNAL ) << "... OK" );
			
			(ros::Duration(2)).sleep();
			channels_enabled = true;
		}
		
		// set the status of the node (disabled)
		cl_bug_switch.call( cmd );
		
		this->enabled = true;
		return true;
	}
	
	/********************************************//**
	 *  
	 * \brief use the algorithm for the 2D navigation
	 * 
	 * The controller first sends the target to the low level using the 
	 * parameter server as required by bug_m; then , it waits for the 
	 * ending signal from the low level. 
	 * 
	 * @param request the objective
	 * @param response the result of the navigation
	 * 
	 * @returns false if the navigation fails or the node is not enabled
	 * 
	 * @attention The waiting is endless if a way to reach the final 
	 * 	target doesn't exist. There's not a timeout. 
	 * 
	 ***********************************************/
	bool perform_navigation( 
		robocluedo_movement_controller_msgs::NavigationService::Request& req,
		robocluedo_movement_controller_msgs::NavigationService::Response& res )
	{
		if( !enabled )
		{
			res.success = false;
			res.details = "(bug_m controller ) the controller isn't active";
			
			return false;
		}
		
		std_srvs::SetBool cmd;
		
		// set the target into the parameter server
		ROS_INFO_STREAM( "(bug_m controller) sending request (" << req.target.x << ", " << req.target.y << ", " << req.target.yaw << ") ... " );
		set_parameter_server( req );
		
		// send the request to the service
		cmd.request.data = true;
		cl_bug_switch.call( cmd );
		
		// wait for the signal from the node
		signal = false;
		ROS_INFO_STREAM( "(bug_m controller) waiting..." );
		wait_for_signal( );
		
		// deactivate the service
		ROS_INFO_STREAM( "(bug_m controller) stopping ..." );
		cmd.request.data = false;
		cl_bug_switch.call( cmd );
		
		res.success = true;
		res.details = "";
		
		return true;
	};
	
	/** turn off the algorithm */
	bool disable( )
	{
		this->enabled = false;
		return true;
	}
	
	/** service for the signal from the bug_m node */
	bool cbk_bug_signal( std_srvs::Empty::Request& req, std_srvs::Empty::Response& res )
	{
		signal=true;
		return true;
	}

private:

	/// channels opened?
	bool channels_enabled = false;
	
	/// client /bug_switch : std_srvs/SetBool
	ros::ServiceClient cl_bug_switch;
	
	/// service /bug_m_signal : std_srvs/Empty
	ros::ServiceServer cl_bug_signal;
	
	/// received a signal from the node
	bool signal = false;
	
	/** set the target into the parameter server */
	void set_parameter_server( robocluedo_movement_controller_msgs::NavigationService::Request& req )
	{
		// des_pos_x
		ros::param::set( "des_pos_x", req.target.x );
		
		// des_pos_y
		ros::param::set( "des_pos_y", req.target.y );
		
		// des_yaw
		ros::param::set( "des_yaw", req.target.yaw );
		
	}
	
	/** wait for the signal */
	void wait_for_signal( )
	{
		ros::Rate r( 20 );
		do
			r.sleep( );
		while( !this->signal && ros::ok( ) );
	}
};


/********************************************//**
 *  
 * \brief move_base navigation
 * 
 * this class implements the workflow with the navigation algorithm 
 * implemented in the node 'move_base_nav.py'. The way it works is 
 * quite similar to the one of 'bug_m' except for the different
 * navigation system. 
 * 
 * The implementation turns on the channels at the first enable call, and
 * leaves them active for the entire lifecycle of the navigation manager. 
 * 
 * \note this kind of navigation is particularly indicated for indoor
 * environments. 
 * 
 ***********************************************/
class nav_move_base : public navigation_controller
{
public:
	
	/** class constructor */
	nav_move_base( std::string name = "", int code = -1 ) :
		navigation_controller( name, code )
	{ 
		// ...
	}
	
	/** class destructor */
	~nav_move_base( )
	{ 
		// ...
	}
	
	/** turn on the algorithm */
	bool enable( )
	{
		std_srvs::SetBool cmd;
		cmd.request.data = false;
		
		if( !channels_enabled )
		{
			// open the client /nav_stack_switch : std_srvs/SetBool
			WTLOG( "Opening client " << LOGSQUARE( SERVICE_MOVE_BASE_SWITCH ) << "..." );
			cl_move_base_switch = nh.serviceClient<std_srvs::SetBool>( SERVICE_MOVE_BASE_SWITCH );
			if( !cl_move_base_switch.waitForExistence( ros::Duration( 60 ) ) )
			{
				WTERR( "unable to contact the server " << SERVICE_MOVE_BASE_SWITCH << " - timeout expired (60s) " );
				return false;
			}
			WTLOG( "Opening client " << LOGSQUARE( SERVICE_MOVE_BASE_SWITCH ) << "... OK" );
			
			// open the service /nav_stack_signal : std_srvs/Empty
			WTLOG( "advertising service " << LOGSQUARE( SERVICE_MOVE_BASE_SIGNAL ) << "..." );
			srv_move_base_signal = nh.advertiseService( SERVICE_MOVE_BASE_SIGNAL, &nav_move_base::cbk_bug_signal, this );
			WTLOG( "advertising service " << LOGSQUARE( SERVICE_MOVE_BASE_SIGNAL ) << "... OK" );
			
			(ros::Duration(2)).sleep();
			channels_enabled = true;
		}
		
		// set the status of the node (disabled)
		cl_move_base_switch.call( cmd );
		
		this->enabled = true;
		return true;
	}
	
	/********************************************//**
	 *  
	 * \brief use the algorithm for the 2D navigation
	 * 
	 * The controller first sends the target to the low level using the 
	 * parameter server as required by move_base_nav; then , it waits for the 
	 * ending signal from the low level. 
	 * 
	 * @param request the objective
	 * @param response the result of the navigation
	 * 
	 * @returns false if the navigation fails or the node is not enabled
	 * 
	 ***********************************************/
	bool perform_navigation( 
		robocluedo_movement_controller_msgs::NavigationService::Request& req,
		robocluedo_movement_controller_msgs::NavigationService::Response& res )
	{
		if( !enabled )
		{
			res.success = false;
			res.details = "(move_base_nav controller ) the controller isn't active";
			
			return false;
		}
		
		std_srvs::SetBool cmd;
		
		// set the target into the parameter server
		ROS_INFO_STREAM( "(move_base_nav controller ) sending request (" << req.target.x << ", " << req.target.y << ", " << req.target.yaw << ") ... " );
		set_parameter_server( req );
		
		// send the request to the service
		cmd.request.data = true;
		cl_move_base_switch.call( cmd );
		
		// wait for the signal from the node
		signal = false;
		ROS_INFO_STREAM( "(move_base_nav controller ) waiting..." );
		wait_for_signal( );
		
		// deactivate the service
		ROS_INFO_STREAM( "(move_base_nav controller ) stopping ..." );
		cmd.request.data = false;
		cl_move_base_switch.call( cmd );
		
		res.success = true;
		res.details = "";
		
		return true;
	};
	
	/** turn off the algorithm */
	bool disable( )
	{
		this->enabled = false;
		return true;
	}
	
	/** service for the signal from the move_base_nav node */
	bool cbk_bug_signal( std_srvs::Empty::Request& req, std_srvs::Empty::Response& res )
	{
		signal=true;
		return true;
	}

private:

	/// channels opened?
	bool channels_enabled = false;
	
	/// client /nav_stack_switch : std_srvs/SetBool
	ros::ServiceClient cl_move_base_switch;
	
	/// service /nav_stack_signal : std_srvs/Empty
	ros::ServiceServer srv_move_base_signal;
	
	/// received a signal from the node
	bool signal = false;
	
	/** set the target into the parameter server */
	void set_parameter_server( robocluedo_movement_controller_msgs::NavigationService::Request& req )
	{
		// des_pos_x
		ros::param::set( "des_pos_x", req.target.x );
		
		// des_pos_y
		ros::param::set( "des_pos_y", req.target.y );
		
		// des_yaw
		ros::param::set( "des_yaw", req.target.yaw );
		
	}
	
	/** wait for the signal */
	void wait_for_signal( )
	{
		ros::Rate r( 20 );
		do
			r.sleep( );
		while( !this->signal && ros::ok( ) );
	}
};



/********************************************//**
 *  
 * \brief implementation of the node as class
 * 
 * The navigation manager uses the *controllers*, i.e. modules (objects) using the low-level implementation of the navigation in order to reach the objective provided. In particular, here are some characteristics of this particular system:
 * 
 * - the navigation manager is a *list of controllers* with one controller active set apart from the array<br>
 * - each controller can be enabled or disabled<br>
 * - all the services and topics needed to make the low level work are hidden in the controller, whereas the navigation manager sees alway the same interface<br>
 * - a controller can be replaced *at runtime* depending on the situation, if needed, which means that the robot can use many combinations of the low level behaviours in order to carry out the objective<br>
 * 
 * see the documentationof the package for further informations. 
 * 
 ***********************************************/
class class_navigation_manager
{
public:
	
	/********************************************//**
	 *  
	 * \brief class constructor
	 * 
	 * The cnode class constructor opens only two services: the first one 
	 * used for selecting the algorithm among the ones registered; and
	 * the second one, for performing the navigation in a synchronous way. 
	 * 
	 ***********************************************/
	class_navigation_manager( )
	{
		// open the service for setting the navigation algorithm to use
		WTLOG( "Advertising service " << LOGSQUARE( SERVICE_SET_ALGORITHM  ) << "..." );
		srv_set_algorithm = nh.advertiseService( SERVICE_SET_ALGORITHM, &class_navigation_manager::cbk_set_algorithm, this );
		WTLOG( "Advertising service " << LOGSQUARE( SERVICE_SET_ALGORITHM  ) << "... OK" );
		
		// open the main interface for the navigation
		WTLOG( "Advertising service " << LOGSQUARE( SERVICE_NAVIGATION  ) << "..." );
		srv_navigation = nh.advertiseService( SERVICE_NAVIGATION, &class_navigation_manager::cbk_navigation, this );
		WTLOG( "Advertising service " << LOGSQUARE( SERVICE_NAVIGATION  ) << "... OK" );
	}
	
	/** class destructor */
	~class_navigation_manager( )
	{
		// ...
	}
	
	/** register one navigation algorithm 
	 * 
	 * The function first of all disables the algorithm provided, then
	 * it allocates its address inside an array of controllers. 
	 * 
	 * @param algo (navigation controller &) the implementation of one algorithm
	 * 
	 * @returns the index associated with the algorithm
	 * 
	 * @note the function will disable the algorithm before registering it
	 * 
	 * @todo check for the null value as argument! Otherwise the function 
	 * 	will try to call disable() on a null, causing a segmentation
	 * 	fault error. 
	 * 
	 * */
	int register_algorithm( navigation_controller *algo )
	{
		// disable it before proceeding
		algo->disable( );
		
		// register the algorithm
		controllers.push_back( algo );
		
		// return its ID
		int id = controllers.size( ) - 1;
		algo->nav_code = id;
		return algo->nav_code;
	}
	
	/** activate one controller
	 * 
	 * the function disables the currently enabled controller, and replaces
	 * that with the selected one, only if feasible. 
	 * 
	 * @param controller_code the index of the controller
	 * 
	 * @returns if the operation succeeded or not
	 * 
	 * */
	bool switch_controller( int controller_code )
	{
		// check the code of the controller
		if( ( controller_code < 0 ) || ( controller_code >= controllers.size( ) ) )
		{
			last_error_explain = SS("selected a undefined controller with ID=") + SSS(controller_code);
			return false;
		}
		
		// try to disable the current controller
		if( running_controller != nullptr )
			if( !running_controller->disable( ) )
			{
				last_error_explain = SS("can't disable the controller with ID=") + SSS(controller_code);
				return false; // unable to stop the controller
			}
		
		// run and enable the controller
		auto temp_running_controller = controllers[controller_code];
		int id = temp_running_controller->nav_code;
		if( temp_running_controller->enable( ) )
		{
			running_controller = temp_running_controller;
			return true;
		}
		else
		{
			last_error_explain = SS("can't enable the controller with ID=") + SSS(id);
			return false; // unable to stop the controller
		}
	}
	
	/** switch the status of the current controller */
	bool set_status( bool new_status )
	{
		// check if there's a current controller
		if( running_controller == nullptr )
		{
			last_error_explain = "running controller is NULL";
			return false;
		}
		
		if( new_status )
			return running_controller->enable( );
		else
			return running_controller->disable( );
	}
	
	/** check wether the currently acivated algorithm is enabled or not */
	bool is_enabled( )
	{
		return( (running_controller!=nullptr) && (running_controller->is_enabled( )) );
	}
	
	/********************************************//**
	 *  
	 * \brief set one algorithm among the available ones and activate it
	 * 
	 * @param request the algorithm to activate (see the details of the message)
	 * @param response the result of the operation
	 * 
	 * @attention you cannot call the navigation until you haven't selected
	 * 	an algorithm with this service!
	 * 
	 ***********************************************/
	bool cbk_set_algorithm( 
		robocluedo_movement_controller_msgs::Algorithm::Request& req,
		robocluedo_movement_controller_msgs::Algorithm::Response& res )
	{
		// check the code of the controller
		if( ( req.algorithm < 0 ) || ( req.algorithm >= controllers.size( ) ) )
		{
			last_error_explain = SS("selected a undefined controller with ID=") + SSS(req.algorithm);
			ROS_WARN_STREAM( "" << last_error_explain );
			
			res.success = false;
			res.details = last_error_explain;
			
			return true;
		}
		
		// select the controller
		if( running_controller==nullptr )
			running_controller = controllers[req.algorithm];
		
		res.success = set_status( req.enabled );
		if( !res.success )
		{
			last_error_explain = SS("cannot enable the controller with ID=") + SSS(req.algorithm);
			ROS_WARN_STREAM( "" << last_error_explain );
		}
		else
			last_error_explain = "";
		res.details = last_error_explain;
		
		return true;
	}
	
	/********************************************//**
	 *  
	 * \brief use the currently running algorithm for the 2D navigation (blocking)
	 * 
	 * the method just tries to pass the calling to the currently active 
	 * controller, if any. 
	 * 
	 * @param request the navigation objective
	 * @param response the result of the operation
	 * 
	 * @attention there must be a controller selected before calling this 
	 * 	function! See the algorithm selection service. 
	 * 
	 ***********************************************/
	
	bool cbk_navigation( 
		robocluedo_movement_controller_msgs::NavigationService::Request& req,
		robocluedo_movement_controller_msgs::NavigationService::Response& res )
	{
		// check if there's a navigation service available
		if( running_controller == nullptr )
		{
			res.success = false;
			res.details = "no navigation service defined";
			ROS_WARN_STREAM( "" << res.details );
			
			return true;
		}
		
		// check if the current service is enabled
		if( !running_controller->is_enabled() )
			if( !req.force_enable )
			{
				res.success = false;
				res.details = "current navigation service not enabled";
				ROS_WARN_STREAM( "" << res.details );
				
				return true;
			}
			else
			{
				// try to enable the service
				if( !running_controller->enable( ) )
				{
					res.success = false;
					res.details = "can't enable the running controller service";
					ROS_WARN_STREAM( "" << res.details );
					
					return true;
				}
			}
		
		// pass the message to the service
		res.success = running_controller->perform_navigation( req, res );
		
		return true;
	};
	
private:
	
	/// ROS Node handle 
	ros::NodeHandle nh;
	
	/// list of navigation algorithms
	std::vector<navigation_controller*> controllers;
	
	/// the currently active controller, if any
	navigation_controller* running_controller = nullptr;
	
	/// explaination of the last error
	std::string last_error_explain = "";
	
	/// algorithm selection service handle
	ros::ServiceServer srv_set_algorithm;
	
	/// navigation service handle
	ros::ServiceServer srv_navigation;
};



/** shudown message */
void shut_msg( int sig )
{
	WTLOG( "stopping... " );
	ros::shutdown( );
}



/********************************************//**
 *  
 * \brief ROS node main - Navigation Manager
 * 
 * Here are instanciated and registered the controllers used for the 
 * navigation. 
 * 
 ***********************************************/
int main( int argc, char* argv[] )
{
	ros::init( argc, argv, NODE_NAME, ros::init_options::NoSigintHandler );
	signal( SIGINT, shut_msg );
	
	ros::NodeHandle nh;
	
	ros::AsyncSpinner spinner(3);
	spinner.start( );
	
	WTLOG( "starting ... " );
	class_navigation_manager nav;
	
	// controller bug_m
	nav_bug_m bug_m;
	bug_m.nav_name = "bug_m";
	nav.register_algorithm( &bug_m );
	
	// controller move_base_nav
	nav_move_base moveb;
	moveb.nav_name = "move_base_nav";
	nav.register_algorithm( &moveb );
	
	WTLOG( "ready" );
	ros::waitForShutdown( );
	
	return 0;
}
