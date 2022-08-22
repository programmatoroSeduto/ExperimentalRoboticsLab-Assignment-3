
/********************************************//**
*  
* @file mission_manager.cpp
* 
* @brief the main node of the architecture, launched at last, and combining
* 	the ROSPlan flow with the aRMOR ontology. 
* 
* During the coding phase, the objective was to not modify this node since
* the version v1.0.0 ... and we did it! With a very little adding. 
* 
* The node implements the high-level mission control of the robot, whereas 
* the ROSPlan framework is used to get the precise plan referred to the
* high-level actions required by the mission manager. The mission manager
* has the main role to combine ROSPlan and the aRMOR ontology, as the 
* navigation unit does, for instance. It is, technically speaking, a 
* state machine, mixing the landmarks of the ROSPlan side with updates
* and queries with aRMOR. 
* 
* An advantage of such approach is to hide the details of the problem: for
* instance, thinking in terms of landmarks instead of entire plan makes
* the system updatable in a simple way, because changing the PDDL doesn't 
* affect the high-level solution. The only condition is that, at least at
* a high level of abstraction, the way to accomplish the objective can be
* described "with the same terms". For instance, the second part of the 
* project used the manipulation action to collect the hints, whereas the
* third one used cameras; but, aat high level of abstraction, the "policy"
* is always the same, so it has been sufficient to change a bit the 
* implementation of the actions dispatched by ROSPlan to make it work. 
* 
* @authors Francesco Ganci
* @version v1.1 
* 
* @note the node supports the develop mode (since version v1.0.0, even if
* not standard)
* 
***********************************************/

#define NODE_NAME "robocluedo_mission_manager"

#define LOGSQUARE( str )  "[" << str << "] "
// #define OUTLABEL       LOGSQUARE( NODE_NAME )
#define OUTLABEL		  NODE_NAME << ": "
#define TLOG( msg )       ROS_INFO_STREAM( OUTLABEL << msg )
#define TWARN( msg )      ROS_WARN_STREAM( OUTLABEL << "WARNING: " << msg )
#define TERR( msg )       ROS_WARN_STREAM( OUTLABEL << "ERROR: " << msg )

#include "ros/ros.h"
#include "signal.h"
#include "robocluedo_rosplan_msgs/RosplanPipelineManagerService.h"
#include "robocluedo_rosplan_msgs/ActionFeedback.h"
#include "robocluedo_armor_msgs/AddHint.h"
#include "robocluedo_armor_msgs/FindConsistentHypotheses.h"
#include "robocluedo_armor_msgs/DiscardHypothesis.h"
#include "robocluedo_armor_msgs/Hypothesis.h"
#include "std_srvs/Trigger.h"
#include "erl2/Oracle.h"
#include "erl2/ErlOracle.h"
#include "std_srvs/SetBool.h"

#include <map>
#include <iostream>

// #define WAITKEY_ENABLED false
#define WAITKEY_ENABLED false
#define DEVELOP_PRINT_ENABLED false

#define WAITKEY { if( WAITKEY_ENABLED ) { std::cout << "press ENTER to continue ... " ; std::cin.get( ) ; std::cout << "go!" << std::endl ; } }
#define WTLOG( msg )  { if( DEVELOP_PRINT_ENABLED ) { ROS_INFO_STREAM( OUTLABEL << msg ); } }
#define WTWARN( msg ) { if( DEVELOP_PRINT_ENABLED ) { ROS_WARN_STREAM( OUTLABEL << msg ); } }
#define WTERR( msg )  { if( DEVELOP_PRINT_ENABLED ) { ROS_ERR_STREAM( OUTLABEL << msg ); } }

#define SERVICE_ROSPLAN_PIPELINE "/robocluedo/pipeline_manager"
#define SERVICE_ARMOR_ADD "/cluedo_armor/add_hint"
#define SERVICE_ARMOR_FIND "/cluedo_armor/find_consistent_h"
#define SERVICE_ARMOR_DEL "/cluedo_armor/wrong_hypothesis"
#define SERVICE_ARMOR_BACKUP "/cluedo_armor/backup"

#define SERVICE_ORACLE_SOLUTION "/oracle_solution"
/*
---
int32 ID
*/

#define TOPIC_ORACLE_HINT "/oracle_hint"
/*
int32 ID
string key
string value
*/

/// how many times the system can fail something
#define MAX_FAULT_COUNT 10

//define LANDMARK_NONE -1

/// landmar REPLAN identifier
#define LANDMARK_REPLAN 0

/// landmark COLLECT identifier
#define LANDMARK_COLLECT 1

/// landmark SOLVE identifier
#define LANDMARK_SOLVE 2

/// landmark REPLAN : the system is restarting the exploration
#define MISSION_STATUS_REPLAN 0

/// landmark COLLECT : the system is sill looking for hypotheses
#define MISSION_STATUS_COLLECT 1

/// landmark SOLVE : the system has a solution to prsent
#define MISSION_STATUS_SOLVE 2

/// propose the solution to the Oracle ans check if the mystery has been solved
#define MISSION_STATUS_ASK_ORACLE 3

/// look for a complete hypothesis inside the ontology
#define MISSION_STATUS_ASK_ONTOLOGY 4

/// receive hints from the Oracle (simple wait)
#define MISSION_STATUS_GET_HINT 5

/// simple wait: the system could fail at most MAX_FAULT_COUNT times
#define MISSION_STATUS_COUNT_FAULT 6

/// the mission has failed
#define MISSION_STATUS_FAULT 7

/// manipulation unit service for auto manipulation
#define SERVICE_RANDOM_ARM_SWITCH "/tip_pos_auto_switch"

class node_mission_manager
{
public:
	
	/** node class constructor (open all the interfaces) */
	node_mission_manager( )
	{
		// rosplan service
		if( !open_client<robocluedo_rosplan_msgs::RosplanPipelineManagerService>( SERVICE_ROSPLAN_PIPELINE, cl_rosplan_pipeline ) )
			return;
		
		// aRMOR clients
		if( !open_client<robocluedo_armor_msgs::AddHint>( SERVICE_ARMOR_ADD, cl_armor_add ) )
			return;
		if( !open_client<robocluedo_armor_msgs::FindConsistentHypotheses>( SERVICE_ARMOR_FIND, cl_armor_find ) )
			return;
		if( !open_client<robocluedo_armor_msgs::DiscardHypothesis>( SERVICE_ARMOR_DEL, cl_armor_del ) )
			return;
		if( !open_client<std_srvs::Trigger>( SERVICE_ARMOR_BACKUP, cl_armor_backup ) )
			return;
		
		// oracle elements
		if( !open_client<erl2::Oracle>( SERVICE_ORACLE_SOLUTION, cl_oracle_solution ) )
			return;
		TLOG( "subscribing to " << TOPIC_ORACLE_HINT << " ... " );
		sub_oracle_hint = nh.subscribe( TOPIC_ORACLE_HINT, 10, &node_mission_manager::cbk_oracle_hint, this );
		TLOG( "subscribing to " << TOPIC_ORACLE_HINT << " ... OK" );
		
		// auto manipulation
		if( !open_client<std_srvs::SetBool>( SERVICE_RANDOM_ARM_SWITCH, cl_auto_manip ) )
			return;
	}
	
	/********************************************//**
	 *  
	 * \brief node working cycle
	 * 
	 * This is where the state machine has been implemented. Currently the
	 * state machine implements 8 states.
	 * 
	 * MISSION_STATUS_REPLAN corresponds to the landmark REPLAN: the node
	 * calls that landmark via pipeline manager (so, planning and dispatch). 
	 * Notice that this action is used also as a intermediate step for passing
	 * from ASK_ONTOLOGY to SOLVE. 
	 * 
	 * MISSION_STATUS_COLLECT is the second landmark COLLECT: the mission 
	 * manager makes a plan towards one waypoint (which one is not important)
	 * and dispatch a plan making the robot to collect the hint in that position.
	 * 
	 * MISSION_STATUS_ASK_ONTOLOGY is performed after the COLLECT. Here 
	 * the mission manager asks to the aRMOR framework if there are consistent
	 * hypotheses to propose to the Oracle. 
	 * 
	 * The common cycle is REPLAN -> COLLECT -> ASK_ONTOLOGY. If there are 
	 * no hypotheses reasy to be proposed to the Oracle, the cycle restarts
	 * from COLLECT, and in case all the waypoints have been explored (in
	 * this case the kb_interface blocks the loading phase because the 
	 * landmark COLLECT is no longer applicable) the cycle restarts 
	 * from REPLAN, then COLLECT and again the same route. 
	 * 
	 * MISSION_STATUS_SOLVE happens when the robot has at least one 
	 * consistent hypothesis. In this case, the robot tries to move to the
	 * center of the environment for proposing its solution to the Oracle.
	 * In case the robot has finished the available paths (topologically
	 * speaking) the transition issues a intermediate MISSION_STATUS_REPLAN
	 * which cleans all the flags for the explored landmarks. The robot 
	 * moves to the center. 
	 * 
	 * the last status is MISSION_STATUS_ASK_ORACLE in which the mission
	 * manager interrogates the Oracle to understand if the solution is 
	 * the winning one, or it is false. if false, the system restarts
	 * from MISSION_STATUS_REPLAN.
	 * 
	 * Something could go wrong for any reason. There are some well known
	 * cases, handled by the policy pointed out until now. But there are 
	 * many other cases in which, for instance, making a service call 
	 * doesn't work, or maybe some expected errors from the planner. 
	 * For these problematic cases, the state machine implements a recovery
	 * policy.
	 * 
	 * the state MISSION_STATUS_COUNT_FAULT decrements a "fault counter", 
	 * which is initialised according to the macro MAX_FAULT_COUNT . The 
	 * next status is decided according to the mission status which has 
	 * generated the error. If the counter becomes zero, the machine
	 * breaks in status MISSION_STATUS_FAULT, meaning that too many errors
	 * have occurred. This strategy is motivated by the fast that sometimes, 
	 * during the tests, it has been observed that when the workload of
	 * the system becomes significant, some service call could fail
	 * unexpectedly. 
	 * 
	 * @note if there are more than one hypothesis, the robot will ask 
	 * for only one of them, then, if the solution is wrong, the robot will
	 * always collect at least one other hint before the other solution
	 * proposal. 
	 * 
	 ***********************************************/
	void spin( )
	{
		// counter of faults
		int fault_count = MAX_FAULT_COUNT;
		
		// next state after fault (-1 if not used)
		int state_after_fault = -1;
		
		// the last complete hypothesis from the Ontology
		robocluedo_armor_msgs::Hypothesis last_hyp;
		
		// status ready to solve
		bool ready_to_solve = false;
		
		// the robot has no more waypoints to explore
		bool last_waypoint = false;
		
		// turn on or off the auto manipulation
		std_srvs::SetBool auto_manip_msg;
		
		WTLOG( "mission manager STARTING WORKING CYCLE" );
		TLOG( "who killed Dr black?" );
		WAITKEY;
		
		while( ros::ok( ) )
		{
			switch( this->mission_status )
			{
			case MISSION_STATUS_REPLAN: // landmark
			{
				WTLOG( "status: " << "MISSION_STATUS_REPLAN" );
				WAITKEY;
				
				// turn off the auto manipulation
				auto_manip_msg.request.data = false;
				cl_auto_manip.call( auto_manip_msg );
				
				// make the plan with LANDMARK_REPLAN
				robocluedo_rosplan_msgs::RosplanPipelineManagerService::Response res = make_plan( LANDMARK_REPLAN );
				
				// check the outcome of the operation
				if( !res.success )
				{
					// inspect the type of failure
					if( !res.success_load_problem )
						TWARN( "unable to load the problem!" );
					else if( !res.success_solve_problem )
					{
						if( res.problem_not_solvable )
							TERR( "VERY UNEXPECTED! REPLAN PROBLEM NOT SOLVABLE!" ); // IT COULD NOT OCCUR! but eventually ...
						else
							TWARN( "unable to solve the plan (unexplained error, maybe from the planner popf?)" );
					}
					else
						TLOG( "error in calling the rosplan pipeline manager" );
					
					state_after_fault = MISSION_STATUS_REPLAN;
					this->mission_status = MISSION_STATUS_COUNT_FAULT;
					
					WAITKEY;
					continue;
				}
				
				// execute the plan
				res = execute_plan( LANDMARK_REPLAN );
				
				// check the outcome of the operation
				if( !res.success )
				{
					// inspect the type of failure
					if( !res.success_parse_plan )
						TWARN( "unable to parse the plan!" );
					else if( !res.success_execute_plan )
						TWARN( "unable to dispatch the problem!" );
					else
						TWARN( "unable to execute (due to unexplained error, maybe the service call?)" );
					
					state_after_fault = MISSION_STATUS_REPLAN;
					this->mission_status = MISSION_STATUS_COUNT_FAULT;
					
					WAITKEY;
					continue;
				}
				
				// check also the goal after the dispatch
				if( res.feedback_received && !res.feedback.goal_achieved )
				{
					TWARN( "UNEXPECTED: not reached the goal at the end of the execution! REPLANNING" );
					
					state_after_fault = MISSION_STATUS_REPLAN;
					this->mission_status = MISSION_STATUS_COUNT_FAULT;
					
					WAITKEY;
					continue;
				}
				
				// next status (REPLAN success)
				if( ready_to_solve )
				{
					WTLOG( "retrying landmark SOLVE after replanning" );
					this->mission_status = MISSION_STATUS_SOLVE;
				}
				else
				{
					WTLOG( "gotta catch 'em all! NA NA NA NA NA NA" );
					this->mission_status = MISSION_STATUS_COLLECT;
				}
				last_waypoint = false;
				
				WAITKEY;
				continue;
			}
			break;
			case MISSION_STATUS_COLLECT: // landmark
			{
				WTLOG( "status: " << "MISSION_STATUS_COLLECT" );
				WAITKEY;
				
				// turn on the auto manipulation
				auto_manip_msg.request.data = true;
				cl_auto_manip.call( auto_manip_msg );
				
				// make the plan with LANDMARK_COLLECT
				robocluedo_rosplan_msgs::RosplanPipelineManagerService::Response res = make_plan( LANDMARK_COLLECT );
				
				// check the outcome of the operation
				if( !res.success )
				{
					// inspect the type of failure
					if( !res.success_load_problem )
					{
						if( res.landmark_is_applicable )
						{
							TWARN( "unable to load the problem! (service failure) RETRYING" );
							state_after_fault = MISSION_STATUS_COLLECT;
							this->mission_status = MISSION_STATUS_COUNT_FAULT;
						}
						else
						{
							WTLOG( "landmark COLLECT no longer applicable (all the waypoints have been explored) TRYING TO directly SOLVE THE MYSTERY" ); 
							this->mission_status = MISSION_STATUS_ASK_ONTOLOGY;
							last_waypoint = true;
						}
					}
					else if( !res.success_solve_problem )
					{
						if( res.problem_not_solvable )
						{
							// each waypoint has been explored (in this case, skip a step)
							TLOG( "Plan not solvable (all the waypoints have been explored) TRYING TO directly SOLVE THE MYSTERY" ); 
							this->mission_status = MISSION_STATUS_ASK_ONTOLOGY;
							last_waypoint = true;
							
						}
						else
						 {
							TWARN( "unable to solve the plan (unexplained, maybe a syntax error in the PDDL files?) REPLANNING" );
							
							state_after_fault = MISSION_STATUS_REPLAN;
							this->mission_status = MISSION_STATUS_COUNT_FAULT;
						}
					}
					else
						TLOG( "error in calling the rosplan pipeline manager" );
					
					WAITKEY;
					continue;
				}
				
				// execute the plan
				res = execute_plan( LANDMARK_COLLECT );
				
				// check the outcome of the operation
				if( !res.success )
				{
					// inspect the type of failure
					if( !res.success_parse_plan )
						TWARN( "unable to parse the plan!" ); // per non saper ne leggere ne scrivere, ti faccio eseguire di nuovo l'azione
					else if( !res.success_execute_plan )
					{
						if( res.feedback.failure )
						{
							if( res.feedback.hw_failure )
							{
								if( res.feedback.failure_nav_system )
									TWARN( "unable to perform navigation (hw navigation problem)" );
								else
									TWARN( "unable to perform manipulation (hw manipulation problem)" );
							}
							else
								TWARN( "unable to dispatch the plan! (failure flag=true ... maybe a problem with services inside robocluedo ROSPlan?)" );
						}
						else
							TWARN( "unable to dispatch the plan! (unexplained)" );
					}
					else
						TWARN( "unable to execute (due to unexplained error, maybe the service call?)" );
					
					state_after_fault = MISSION_STATUS_COLLECT;
					this->mission_status = MISSION_STATUS_COUNT_FAULT;
					
					WAITKEY;
					continue;
				}
				
				// check also the goal after the dispatch
				if( res.feedback_received && !res.feedback.goal_achieved )
				{
					TWARN( "UNEXPECTED: not reached the goal at the end of the execution! REPLANNING" );
					
					state_after_fault = MISSION_STATUS_REPLAN;
					this->mission_status = MISSION_STATUS_COUNT_FAULT;
					
					WAITKEY;
					continue;
				}
				
				// next status
				this->mission_status = MISSION_STATUS_ASK_ONTOLOGY;
				
				WAITKEY;
				continue;
			}
			break;
			case MISSION_STATUS_ASK_ONTOLOGY: // armor
			{
				WTLOG( "status: " << "MISSION_STATUS_ASK_ONTOLOGY" );
				WAITKEY;
				
				// ask the ontology for any complete hypothesis
				robocluedo_armor_msgs::FindConsistentHypotheses find_req;
				
				if( !cl_armor_find.call( find_req ) )
				{
					TWARN( "unable to call aRMOR for finding hypotheses! RETRYING" );
					
					state_after_fault = MISSION_STATUS_ASK_ONTOLOGY;
					this->mission_status = MISSION_STATUS_COUNT_FAULT;
					
					WAITKEY;
					continue;
				}
				
				// check the number of returned hints from the ontology
				TLOG( "number of bright ideas until now : " << find_req.response.hyp.size( )  );
				if( find_req.response.hyp.size( ) > 0 )
				{
					ready_to_solve = true;
					last_hyp = find_req.response.hyp[0]; // take the first returned
					this->mission_status = MISSION_STATUS_SOLVE;
				}
				else
				{
					ready_to_solve = false;
					
					if(last_waypoint)
					{
						WTLOG( "no brilliant ideas from the RoboCLuedo B.B.B. (Big Brain Bruh) REPLANNING" );
						this->mission_status = MISSION_STATUS_REPLAN;
					}
					
					else
					{
						WTLOG( "no brilliant ideas from the RoboCLuedo B.B.B. (Big Brain Bruh) COLLECTING AGAIN" );
						this->mission_status = MISSION_STATUS_COLLECT;
					}
				}
				
				continue;
			}
			break;
			case MISSION_STATUS_SOLVE: // landmark
			{
				WTLOG( "status: " << "MISSION_STATUS_SOLVE" );
				WAITKEY;
				
				// turn off the auto manipulation
				auto_manip_msg.request.data = false;
				cl_auto_manip.call( auto_manip_msg );
				
				// check ready to solve (just to be super sure)
				if( !ready_to_solve )
				{
					TWARN( "HEYHEYHEY WHAT ARE YOU DOING HERE? NOT READY TO SOLVE! " );
					
					ready_to_solve = false;
					this->mission_status = MISSION_STATUS_COLLECT;
					
					WAITKEY;
					continue;
				}
				
				// make the plan with LANDMARK_SOLVE
				robocluedo_rosplan_msgs::RosplanPipelineManagerService::Response res = make_plan( LANDMARK_SOLVE );
				
				// inspect the error if any
				if( !res.success )
				{
					// inspect the type of failure
					if( !res.success_load_problem )
					{
						TWARN( "unable to load the problem! RETRYING" );
						state_after_fault = MISSION_STATUS_SOLVE;
						this->mission_status = MISSION_STATUS_COUNT_FAULT;
					}
					else if( !res.success_solve_problem )
					{
						// here the problem could result unsolvable due to the topology of the environment
						//    and the (correct) use of the predicate (explored ?wp)
						//    (see development nodes)
						// in this case, the best solution is to REPLAN then SOLVE
						
						if( res.problem_not_solvable )
						{
							// TWARN( "UNEXPECTED! Plan not solvable, can't find a solution (maybe a issue with the PDDL model?)" ); 
							
							// here the problem could result unsolvable due to the topology of the environment
							//    and the (correct) use of the predicate (explored ?wp)
							//    (see development log)
							// in this case, the best solution is to REPLAN then SOLVE
							
							WTLOG( "plan not solvable (need intermediate replanning)" );
							ready_to_solve = true; // just to be sure ...
							this->mission_status = MISSION_STATUS_REPLAN;
							
							WAITKEY;
							continue;
						}
						else
							TWARN( "unable to solve the plan (unexplained, maybe service faults?) REPLANNING" );
						
						
						ready_to_solve = false;
						state_after_fault = MISSION_STATUS_REPLAN;
						this->mission_status = MISSION_STATUS_COUNT_FAULT;
					}
					else
						TLOG( "error in calling the rosplan pipeline manager" );
					
					WAITKEY;
					continue;
				}
				
				// execute the plan
				res = execute_plan( LANDMARK_SOLVE );
				
				// inspect the error if any
				if( !res.success )
				{
					// inspect the type of failure
					if( !res.success_parse_plan )
					{
						TWARN( "unable to parse the plan!" ); // per non saper ne leggere ne scrivere, ti faccio eseguire di nuovo l'azione
						
						state_after_fault = MISSION_STATUS_SOLVE;
						this->mission_status = MISSION_STATUS_COUNT_FAULT;
						
						WAITKEY;
						continue;
					}
					else if( !res.success_execute_plan )
					{
						if( res.feedback.failure )
						{
							if( res.feedback.hw_failure )
								TWARN( "unable to perform navigation (hw navigation problem" << (res.feedback.failure_manipulation ? ", strange: failure_manipulation=true" : "") << ")" );
							else
								TWARN( "unable to dispatch the plan! (hw_failure=false but failure=true ... maybe a problem with services inside robocluedo ROSPlan?)" );
						}
						else
							TWARN( "unable to dispatch the plan! (unexplained, no hw failure)" );
					}
					else
						TWARN( "unable to execute due to unexplained error (maybe did a service call fail?)" );
					
					ready_to_solve = false;
					state_after_fault = MISSION_STATUS_COLLECT;
					this->mission_status = MISSION_STATUS_COUNT_FAULT;
					
					WAITKEY;
					continue;
				}
				
				// check the goal
				if( res.feedback_received && !res.feedback.goal_achieved )
				{
					TWARN( "UNEXPECTED: not reached the goal at the end of the execution! REPLANNING" );
					
					ready_to_solve = false;
					state_after_fault = MISSION_STATUS_REPLAN;
					this->mission_status = MISSION_STATUS_COUNT_FAULT;
					
					WAITKEY;
					continue;
				}
				
				// next mission status
				this->mission_status = MISSION_STATUS_ASK_ORACLE;
				
				WAITKEY;
				continue;
			}
			break;
			case MISSION_STATUS_ASK_ORACLE: // oracle, armor
			{
				WTLOG( "status: " << "MISSION_STATUS_ASK_ORACLE" );
				WAITKEY;
				
				// ask the Oracle for the solution of the mystery
				erl2::Oracle solution;
				if( !cl_oracle_solution.call( solution ) )
				{
					TWARN( "unable to contact the oracle -- RETRYING" );
					
					state_after_fault = MISSION_STATUS_ASK_ORACLE;
					this->mission_status = MISSION_STATUS_COUNT_FAULT;
					
					WAITKEY;
					continue;
				}
				
				// check the solution
				TLOG( "\n\tI see everybody here, in the " << last_hyp.where << ", to discover who is the killer. \n\tWell, I caught that." );
				( ros::Duration( 1 ) ).sleep();
				TLOG( "\n\tExactly, " << last_hyp.who << ", YOU ARE THE KILLER; right?" );
				
				// suspence
				std::cout << "(suspence...)" << std::endl;
				( ros::Duration( (rand()%4) +1 ) ).sleep();
				
				WTLOG( "from the Oracle: ID[" << solution.response.ID  << "]" );
				if( solution.response.ID == last_hyp.ID )
				{
					std::cout << last_hyp.who << ": \n\t" << "YES RoboCLuedo, I killed Dr Black! With a " << last_hyp.what << " here! He deserved it! MUHAHAHAH" << std::endl;
					( ros::Duration( 0.2 ) ).sleep();
					TLOG( "\n\tPolice! Arrest the killer!" );
					( ros::Duration( 0.1 ) ).sleep();
					std::cout << last_hyp.who << ": \n\t" << "I will go back, RoboCLuedo! I gonna kill you all! MUHAHAH" << std::endl;
					( ros::Duration( 0.5 ) ).sleep();
					TLOG( "\n\tanother mystery solved thanks to my wit. " );
					
					( ros::Duration( 2 ) ).sleep();
					
					WTLOG( "==== ID=" << last_hyp.ID << " | where[" << last_hyp.where << "] what[" << last_hyp.what << "] who[" << last_hyp.who << "] ====" );
					
					WAITKEY;
					return;
				}
				else
				{
					std::cout << last_hyp.who << ": \n\t" << "NO RoboCLuedo, I'm not the killer. " << std::endl;
					( ros::Duration( 0.5 ) ).sleep();
					std::cout << "OOOOOOOOOOOoooooouooooouuuuuuuuhhhhhh" << std::endl;
					( ros::Duration( 2 ) ).sleep();
					
					TLOG( "\n\tUnderstood. \n\tWho killed Dr. Black is still there, in this room, in this house.\n\t WE MUST FIND THAT." );
					( ros::Duration( 0.5 ) ).sleep();
					std::cout << last_hyp.who << ": \n\t" << "hey, where did the majordomo go?" << std::endl;
					( ros::Duration( 0.2 ) ).sleep();
					
					// mark the hint as wrong
					robocluedo_armor_msgs::DiscardHypothesis discard_note;
					discard_note.request.ID = last_hyp.ID;
					
					if( !cl_armor_del.call( discard_note ) )
					{
						TWARN( "unable to discard the ID -- can't contact the armor service" );
					}
					else
					{
						std_srvs::Trigger backup_cmd;
						if( !cl_armor_backup.call( backup_cmd ) || !backup_cmd.response.success )
						{
							WTWARN( "ontologybackup failed (retrying later ...)" );
						}
					}
					
					ready_to_solve = false;
					this->mission_status = MISSION_STATUS_REPLAN;
					
					WAITKEY;
					continue;
				}
			}
			break;
			case MISSION_STATUS_COUNT_FAULT: 
			{
				--fault_count;
				TWARN( "status: " << "MISSION_STATUS_COUNT_FAULT (remaining " << fault_count << ")" );
				WAITKEY;
				
				if( fault_count <= 0 )
				{
					mission_status = MISSION_STATUS_FAULT;
					continue;
				}
				
				// next status depending on the current state
				mission_status = state_after_fault;
				continue;
			}
			break;
			
			case MISSION_STATUS_FAULT:
			{
				TERR( "status: " << "MISSION_STATUS_FAULT" );
				WAITKEY;
				
				TERR( "mission failed : too much errors, counter was zero" );
				return;
			}
			break;
			default:
			{
				TWARN( "unknown mission status (obtained " << mission_status << ") REPLANNING" );
				mission_status = MISSION_STATUS_COUNT_FAULT;
				state_after_fault = MISSION_STATUS_REPLAN;
				
				ready_to_solve = false;
				
				WAITKEY;
				continue;
			}
			}
		}
	}
	
	/** make a plan ready to be executed (and call the service) */
	robocluedo_rosplan_msgs::RosplanPipelineManagerService::Response make_plan( int landmark )
	{
		robocluedo_rosplan_msgs::RosplanPipelineManagerService cmd = create_planning_request( landmark );
		
		// send the request and wait
		if( !cl_rosplan_pipeline.call( cmd ) )
		{
			TWARN( "unable to make a plan! client calling failed" );
			set_standard_response( cmd, true );
		}
		
		return cmd.response;
	}
	
	/** execute the plan */
	robocluedo_rosplan_msgs::RosplanPipelineManagerService::Response execute_plan( int landmark )
	{
		robocluedo_rosplan_msgs::RosplanPipelineManagerService cmd = create_exec_request( landmark );
		
		// send the request and wait
		if( !cl_rosplan_pipeline.call( cmd ) )
		{
			TWARN( "unable to execute the plan! client calling failed" );
			set_standard_response( cmd, true );
		}
		
		return cmd.response;
	}
	
	/********************************************//**
	 *  
	 * \brief receive a hint from the Oracle and directly store it
	 * 
	 * The callback receives a hit from the Oracle (or, in the third part 
	 * of the RoboCLuedo project) and directly stores it into the aRMOR
	 * ontology. This function also checks whether a hint is valid or
	 * not before adding it into the database. 
	 * 
	 * The callback could also ask for a backup of the ontology.
	 * 
	 * @param hint the hint from the oracle (or from the decoder)
	 * 
	 ***********************************************/
	void cbk_oracle_hint( const erl2::ErlOracle::ConstPtr& hint )
	{
		TLOG( "A clue, RoboWatson! It says (ID=" << hint->ID << ", key=" << hint->key << ", value=" << hint->value << ")" );
		
		// check validity
		if(is_valid_hint( hint ))
		{
			// store it into the ontology
			robocluedo_armor_msgs::AddHint addh;
			addh.request.hypID = hint->ID;
			addh.request.property = hint->key;
			addh.request.Belem = hint->value;
			
			if( !cl_armor_add.call( addh ) )
			{
				TWARN( "(cbk_oracle_hint, add hint) can't reach the service" );
				return;
			}
			
			if( !addh.response.success )
			{
				TWARN( "(cbk_oracle_hint, add hint) aRMOR interface replied with success=false" );
				return;
			}
			
			if( (rand()%2) > 0 )
				TLOG( "Seems reasonable." );
			else
				std::cout << "\t(RoboWatson is taking notes...)" << std::endl;
			
			std_srvs::Trigger backup_cmd;
			if( !cl_armor_backup.call( backup_cmd ) || !backup_cmd.response.success )
			{
				TWARN( "ontologybackup failed (retrying later ...)" );
			}
		}
		else
			TLOG( "It doesn't make sense." );
	}
	
	/** check wether the hint is valid or not */
	bool is_valid_hint( const erl2::ErlOracle::ConstPtr& hint )
	{
		return (
			(hint->ID >= 0) &&
			(hint->key != "") &&
			(hint->value != "") &&
			(hint->key != "-1") &&
			(hint->value != "-1")
		);
	}
	
	/** make a planning request without parsing and execution */
	robocluedo_rosplan_msgs::RosplanPipelineManagerService create_planning_request( int landmark )
	{
		robocluedo_rosplan_msgs::RosplanPipelineManagerService cmd;
		
		set_request( cmd, true, true, false, false, landmark );
		set_standard_response( cmd, false );
		
		return cmd;
	}
	
	/** make a planning request without loading and solving */
	robocluedo_rosplan_msgs::RosplanPipelineManagerService create_exec_request( int landmark = -1 )
	{
		robocluedo_rosplan_msgs::RosplanPipelineManagerService cmd;
		
		set_request( cmd, false, false, true, true, landmark );
		set_standard_response( cmd, false );
		
		return cmd;
	}
	
	/** set a standard response for the service planning pipeline request */
	void set_standard_response( robocluedo_rosplan_msgs::RosplanPipelineManagerService& cmd, bool failure = false )
	{
		cmd.response.success = !failure;
		
		cmd.response.success_load_problem = true;
		cmd.response.landmark_is_applicable = true;
		cmd.response.success_solve_problem = true;
		cmd.response.success_parse_plan = true;
		cmd.response.success_execute_plan = true;
		
		cmd.response.problem_not_solvable = false;
		
		cmd.response.feedback_received = !failure;
		
		cmd.response.feedback.action_name = "";
		cmd.response.feedback.goal_achieved = false;
		cmd.response.feedback.failure = failure;
		cmd.response.feedback.hw_failure = false;
		cmd.response.feedback.failure_nav_system = false;
		cmd.response.feedback.failure_manipulation = false;
		cmd.response.feedback.details = "";
	}
	
	/** quick set request */
	void set_request( robocluedo_rosplan_msgs::RosplanPipelineManagerService& cmd, 
		bool load, bool solve, bool parse, bool exec, int landmark )
	{
		cmd.request.landmark = landmark;
		
		cmd.request.load_problem = load;
		cmd.request.solve_problem = solve;
		cmd.request.parse_plan = parse;
		cmd.request.execute_plan = exec;
	}
	
private:
	
	/// the node handle
	ros::NodeHandle nh;
	
	/// auto manipulation client
	ros::ServiceClient cl_auto_manip;
	
	/// client rosplan pipeline manager
	ros::ServiceClient cl_rosplan_pipeline;
	
	/// client for armor add hint
	ros::ServiceClient cl_armor_add;
	
	/// client for armor find consistent hypotheses
	ros::ServiceClient cl_armor_find;
	
	/// client for armor discard hypotheses
	ros::ServiceClient cl_armor_del;
	
	/// client for armo backup request
	ros::ServiceClient cl_armor_backup;
	
	/// client for requiring the solution to the oracle
	ros::ServiceClient cl_oracle_solution;
	
	/// subscription oracle hints
	ros::Subscriber sub_oracle_hint;
	
	/// state of the mission
	int mission_status = MISSION_STATUS_REPLAN;
	
	/** open a client */
	template<class T>
	bool open_client( std::string cl_name, ros::ServiceClient& cl_handle )
	{
		TLOG( "Opening client " << LOGSQUARE( cl_name ) << "..." );
		cl_handle = nh.serviceClient<T>( cl_name );
		if( !cl_handle.waitForExistence( ros::Duration( 60 ) ) )
		{
			TERR( "unable to contact the server " << cl_name << " - timeout expired (60s) " );
			return false;
		}
		TLOG( "Opening client " << LOGSQUARE( cl_name ) << "... OK" );
		
		return true;
	}
};



void shut_msg( int sig )
{
	TLOG( "stopping... " );
	ros::shutdown( );
}


int main( int argc, char* argv[] )
{
	ros::init( argc, argv, "robocluedo_mission_manager", ros::init_options::NoSigintHandler );
	signal( SIGINT, shut_msg );
	ros::NodeHandle nh;
	
	ros::AsyncSpinner spinner( 5 );
	spinner.start( );
	
	TLOG( "starting ... " );
	node_mission_manager node;
	
	TLOG( "ready" );
	node.spin( );
	
	return 0;
}
