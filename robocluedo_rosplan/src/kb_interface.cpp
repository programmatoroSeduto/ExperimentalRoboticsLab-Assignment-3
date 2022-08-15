
#define NODE_NAME "kb_interface"

/********************************************//**
*  
* @file kb_interface.cpp
* @brief services for read/write on the rosplan kb
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


/// representation of one predicate
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


class kb_interface : kb_tools
{
public:
	
	/// kb_interface node constructor
	kb_interface( ) : kb_tools( )
	{
		// waypoints (center excluded)
		wps.push_back("wp1");
		wps.push_back("wp2");
		wps.push_back("wp3");
		wps.push_back("wp4");
		
		
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
	
	/// replan service
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
	
	/// update goal service
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
	TLOG( "stopping... " );
	ros::shutdown( );
}


int main( int argc, char* argv[] )
{
	ros::init( argc, argv, NODE_NAME, ros::init_options::NoSigintHandler );
	signal( SIGINT, shut_msg );
	
	ros::NodeHandle nh;
	
	TLOG( "starting ... " );
	
	kb_interface kbi;
	
	TLOG( "Advertising service " << LOGSQUARE( SERVICE_REPLAN  ) << "..." );
	ros::ServiceServer tsrv_replan = nh.advertiseService( SERVICE_REPLAN, &kb_interface::cbk_replan, &kbi );
	srv_replan = &tsrv_replan;
	TLOG( "Advertising service " << LOGSQUARE( SERVICE_REPLAN ) << "... OK" );
	
	TLOG( "Advertising service " << LOGSQUARE( SERVICE_UPDATE_GOAL  ) << "..." );
	ros::ServiceServer tsrv_update_goal = nh.advertiseService( SERVICE_UPDATE_GOAL, &kb_interface::cbk_update_goal, &kbi );
	srv_update_goal = &tsrv_update_goal;
	TLOG( "Advertising service " << LOGSQUARE( SERVICE_UPDATE_GOAL ) << "... OK" );
	
	TLOG( "ready" );
	ros::spin( );
	
	return 0;
}
