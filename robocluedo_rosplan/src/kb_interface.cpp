
#define NODE_NAME "kb_interface"

/********************************************//**
*  
* @file kb_interface.cpp
* @brief services for read/write on the rosplan kb
* 
* This node exposes, as ROS service, some important functionalities for
* performing the replanning. 
* 
* Its role is mostly to hide the specificiy of some corrections on the
* knowledge base, due to the structure of the PDDL model in use for this
* project. 
* 
* @note It supports the develop mode: waitkey, develop print
* 
* @authors Francesco Ganci
* @version v1.0
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
#include "std_srvs/Empty.h"
#include "knowledge_base_tools/kb_tools.h"
#include "robocluedo_rosplan_msgs/UpdateGoal.h"

#include <string>
#include <map>
#include <signal.h>

#define SERVICE_REPLAN "/replan"
ros::ServiceServer *srv_replan;

#define SERVICE_UPDATE_GOAL "/update_goal"
ros::ServiceServer *srv_update_goal;

#define LANDMARK_REPLAN  0
#define LANDMARK_COLLECT 1
#define LANDMARK_SOLVE   2


/********************************************//**
 *  
 * \brief representation of a predicate
 * 
 * This simple data structure is used for handling the predicates in a more
 * convenient way. 
 * 
 * @todo this class should be better designed, or, even better, included
 * 	into the currently available framework for the knowledge base.
 * 
 ***********************************************/
class predicate_instance
{
public:
	
	/// name
	std::string name = "";
	
	/// parameters
	std::map<std::string, std::string> params;
	
	/// default value
	bool default_value = false;
	
	/// insert one param
	void set_param( std::string p1, std::string val1 )
	{
		params[p1] = val1;
	}
	
	/// insert two params
	void set_param_2( 
		std::string p1, std::string val1,
		std::string p2, std::string val2 )
	{
		params[p1] = val1;
		params[p2] = val2;
	}
};


/********************************************//**
 *  
 * \brief Implementation of the node kb_interface as class
 * 
 * The node provides a general service for replanning, as well as some
 * utilities taking into account the particularities of the landmarks
 * with respect to the structure of the PDDL model. 
 * 
 * @todo the node should read the waypoints from the PDDL model; currently,
 * 	the waypoints are hardcoded in the constructor. 
 * 
 * @todo the topology of the problem can be better handled by this node. 
 * 
 ***********************************************/
class kb_interface : kb_tools
{
public:
	
	/********************************************//**
	 *  
	 * \brief kb_interface node constructor
	 * 
	 * The constructor allocates the most relevant predicates of the PDDL
	 * model and their default values. These values are used when a 
	 * landmark, or a replanning, is required by another node. 
	 * 
	 * @todo the node should read the waypoints from the PDDL model; currently,
	 * 	the waypoints are hardcoded in the constructor. 
	 * 
	 ***********************************************/
	kb_interface( ) : kb_tools( )
	{
		// waypoints (center excluded)
		wps.push_back("wp1");
		wps.push_back("wp2");
		wps.push_back("wp3");
		wps.push_back("wp4");
		wps.push_back("wp5");
		wps.push_back("wp6");
		
		// === init predicates === //
		
		// flag "dirty"
		{ predicate_instance p; p.name = "dirty"; p.default_value = true; 
			init_pred.push_back(p); }
		{ predicate_instance p; p.name = "not-dirty"; p.default_value = false; 
			init_pred.push_back(p); }
		
		// DON'T SET THE ENVIRONMENT AND NOT THE LOCATION
		
		// hint ready flag
		{ predicate_instance p; p.name = "hint-ready"; p.default_value = false; 
			init_pred.push_back(p); }
		{ predicate_instance p; p.name = "not-hint-ready"; p.default_value = true; 
			init_pred.push_back(p); }
		
		// hint collected flag for each waypoint
		for(std::string& wp : wps)
		{
			{ predicate_instance p; p.name = "not-hint-collected"; p.default_value = true; p.set_param("wp", wp);
				init_pred.push_back(p); }
			{ predicate_instance p; p.name = "hint-collected"; p.default_value = false; p.set_param("wp", wp);
				init_pred.push_back(p); }
		}
		
		
		// === goal predicates === //
		
		// flag "dirty"
		{ predicate_instance p; p.name = "dirty"; goal_pred.push_back(p); }
		{ predicate_instance p; p.name = "not-dirty"; goal_pred.push_back(p); }
		
		// center flag
		{ predicate_instance p; p.name = "at-center"; goal_pred.push_back(p); }
		
		// hint ready flag
		{ predicate_instance p; p.name = "hint-ready"; goal_pred.push_back(p); }
		{ predicate_instance p; p.name = "not-hint-ready"; goal_pred.push_back(p); }
		
		// predicates depending on the waypoints
		for(std::string& wp : wps)
		{
			{ predicate_instance p; p.name = "not-hint-collected"; p.set_param("wp", wp);
				goal_pred.push_back(p); }
			{ predicate_instance p; p.name = "hint-collected"; p.set_param("wp", wp);
				goal_pred.push_back(p); }
			{ predicate_instance p; p.name = "at"; p.set_param("wp", wp);
				goal_pred.push_back(p); }
		}
	}
	
	/********************************************//**
	 *  
	 * \brief implementation of the replan service
	 * 
	 * the service first of all restores the most relelvand values of the 
	 * PDDL model inside the ontology, then adjusts the informations regarding
	 * the topological localisation in a way such that the only place 
	 * explored is the one where the robot is currently. 
	 * 
	 * @param request (empty)
	 * @param response (empty)
	 * 
	 ***********************************************/
	bool cbk_replan( std_srvs::Empty::Request& req, std_srvs::Empty::Response& res )
	{
		// reset the init state
		for(predicate_instance& p : init_pred)
			this->set_predicate( p.name, p.params, p.default_value );
		
		// reset the navigation 
		if(this->get_predicate( "at-center", {} ))
		{
			std::map<std::string, std::string> m;
			
			// each landmark is not explored
			for(auto& wp : wps)
			{
				m["wp"] = wp;
				this->set_predicate( "explored", m, false );
				this->set_predicate( "not-explored", m, true );
			}
			m["wp"] = "center";
			this->set_predicate( "explored", m, true );
			
			return true;
		}
		else
		{
			std::map<std::string, std::string> m;
			m["wp"] = "center";
			
			// the robot is not at the center of the arena
			this->set_predicate( "at-center", {}, false );
			this->set_predicate( "explored", m, false );
			this->set_predicate( "not-explored", m, true );
			
			for(auto& wp : wps)
			{
				m["wp"] = wp;
				
				if(this->get_predicate( "at", m ))
				{
					this->set_predicate( "explored", m, true );
					this->set_predicate( "not-explored", m, false );
				}
				else
				{
					this->set_predicate( "explored", m, false );
					this->set_predicate( "not-explored", m, true );
				}
			}
		}
		
		return true;
	}
	
	/********************************************//**
	 *  
	 * \brief implementation of service for updating goals
	 * 
	 * Purpose of this service is to create the proper conditions in order
	 * to apply one of the three available landmarks: REPLAN, COLLECT and
	 * SOLVE. 
	 * 
	 * "REPLAN" simply sets the "dirty" flags in a way such that the plan,
	 * when found, is going to start with the (replan ) PDDL action. 
	 * The localisation is not affected, or at least not by this node. 
	 * 
	 * "COLLECT" causes the node to look for a still unexplored waypoint.
	 * if all the waypoints have been explored, the node refuses the request,
	 * otherwise it sets the first available waypoint. The command doesn't 
	 * allow to decide which waypoint the robot can explore: the waypoint is 
	 * choosen automatically by this node. 
	 * 
	 * "SOLVE" only sets the "dirty" flags as the "REPLAN" landmark does, 
	 * but in a way such that the system has to necessairly call the 
	 * PDDL action (solve ?wp) after moved to the center of the arena. 
	 * 
	 * @param request it communicates the landmark to set
	 * @param response 
	 * 
	 * @attention the request is rejected with no effect on the knowledge
	 * 	base if the requested landark is not applicable. It could happen for
	 * 	instance when the robot has explored every single waypoint. 
	 * 
	 * @note in any case, the goal is cancelled, hence a failure in applying
	 * 	one unapplicable landmark requires to set again one more landmark.
	 * 
	 ***********************************************/
	bool cbk_update_goal( robocluedo_rosplan_msgs::UpdateGoal::Request& req, robocluedo_rosplan_msgs::UpdateGoal::Response& res )
	{
		// clear the goal
		for(predicate_instance& p : goal_pred)
			this->set_goal( p.name, p.params, false );
		
		// depending on the selected goal...
		switch(req.landmark)
		{
		case LANDMARK_REPLAN :
		{
			// set dirty
			this->set_predicate( "dirty", {}, true );
			this->set_predicate( "not-dirty", {}, false );
			
			// set not dirty as goal
			this->set_goal( "not-dirty", {}, true );
			
			res.success = true;
			res.applicable = true;
		}
		break;
		case LANDMARK_COLLECT :
		{
			std::map<std::string, std::string> m;
			std::string wp_target = "";
			
			// try to find a wp where to go
			for(auto& wp : wps)
			{
				m["wp"] = wp;
				
				// check if the robot is in that wp
				bool at = this->get_predicate( "at", m );
				if(!at)
				{
					// check if there's still a hint there
					bool hint = this->get_predicate( "not-hint-collected", m );
					if(hint)
					{
						// you can go there!
						wp_target = wp;
						break;
					}
				}
			}
			
			if( wp_target != "" )
			{
				// found a target!
				m["wp"] = wp_target;
				this->set_goal( "at", m, true );
				this->set_goal( "hint-collected", m, true );
				this->set_goal( "not-hint-collected", m, false );
			}
			else
			{
				// action failed
				res.success = false;
				res.applicable = false;
				
				return true;
			}
		}
		break;
		case LANDMARK_SOLVE :
		{
			// set dirty
			this->set_predicate( "not-dirty", {}, true );
			this->set_predicate( "dirty", {}, false );
			
			// set dirty as goal
			this->set_goal( "dirty", {}, true );
		}
		break;
		}
		
		res.success = true;
		res.applicable = true;
		
		return true;
	}
	
private:
	
	// ROS node handle
    ros::NodeHandle nh;
	
	/// a list of init predicates
	std::vector<predicate_instance> init_pred;
	
	/// a list of goal predicates
	std::vector<predicate_instance> goal_pred;
	
	/// list of waypoints
	std::vector<std::string> wps;
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
	
	WTLOG( "starting ... " );
	
	kb_interface kbi;
	
	WTLOG( "Advertising service " << LOGSQUARE( SERVICE_REPLAN  ) << "..." );
	ros::ServiceServer tsrv_replan = nh.advertiseService( SERVICE_REPLAN, &kb_interface::cbk_replan, &kbi );
	srv_replan = &tsrv_replan;
	WTLOG( "Advertising service " << LOGSQUARE( SERVICE_REPLAN ) << "... OK" );
	
	WTLOG( "Advertising service " << LOGSQUARE( SERVICE_UPDATE_GOAL  ) << "..." );
	ros::ServiceServer tsrv_update_goal = nh.advertiseService( SERVICE_UPDATE_GOAL, &kb_interface::cbk_update_goal, &kbi );
	srv_update_goal = &tsrv_update_goal;
	WTLOG( "Advertising service " << LOGSQUARE( SERVICE_UPDATE_GOAL ) << "... OK" );
	
	WTLOG( "ready" );
	ros::spin( );
	
	return 0;
}
