#! /usr/bin/env python

import os
import rospy
from std_srvs.srv import Empty, EmptyRequest, EmptyResponse
from std_msgs.msg import String
from rosplan_dispatch_msgs.srv import DispatchService, DispatchServiceRequest, DispatchServiceResponse
from robocluedo_rosplan_msgs.msg import ActionFeedback
from robocluedo_rosplan_msgs.srv import RosplanPipelineManagerService, RosplanPipelineManagerServiceRequest, RosplanPipelineManagerServiceResponse
from robocluedo_rosplan_msgs.srv import UpdateGoal, UpdateGoalRequest, UpdateGoalResponse
from rosplan_dispatch_msgs.msg import CompletePlan

NODE_NAME = "rosplan_pipeline_manager"
''' nothing to say, node name
'''

SRV_TIMEOUT = rospy.Duration(60)
''' a timeout of 1min used for the client connection
'''

LANDMARK_REPLAN = 0
''' landmark corresponding to : replan
'''

LANDMARK_COLLECT = 1
''' landmark corresponding to : collect
'''

LANDMARK_SOLVE = 2
''' landmark corresponding to : solve
'''

service_problem = "/rosplan_problem_interface/problem_generation_server"
''' name of the trigger of problem interface
'''

cl_problem = None
''' (ros client handle) problem instance trigger
'''

topic_problem_instance = "/rosplan_problem_interface/problem_instance"
''' topic used to understand if the problem generation succeeded
'''

sub_problem_instance = None
''' (ros subscriber handle) 
'''

problem_instance_received = False
''' this flag is set true when the subscriber received a problem instance.
	this means that the problem generation worked as expected. 
'''

problem_instance_text = ""
''' the last problem instance received
'''

service_plan = "/rosplan_planner_interface/planning_server"
''' name of the trigger of planning interface
'''

cl_plan = None
''' (ros client handle) planning interface trigger
'''

service_parse = "/rosplan_parsing_interface/parse_plan"
''' name of the trigger of the plan parser
'''

cl_parse = None
''' (ros client handle) plan parser trigger
'''

topic_planner_interface = "/rosplan_planner_interface/planner_output"
''' used for checking if the plan succeeded
'''

sub_planner_interface = None
''' (ros subscription handle)
'''

planner_interface_received = False
''' it becomes false a bit before triggering the planner,
	and true when the planning interface succeeded.
'''

planner_interface_text = ""
''' text received from the planning interface
'''

planner_interface_solution_path = ""
''' the value of the parameter "output_problem_path"
'''

topic_parser_interface = "/rosplan_parsing_interface/complete_plan"
''' used for checking the outcome of the parsing interface
''' 

sub_parser_interface = None
''' (ros subscription handle)
''' 

parsing_interface_received = False
''' if the parsed plan han been sent or not
''' 

parsing_interface_array = []
''' the parsed plan
''' 

service_dispatch = "/rosplan_plan_dispatcher/dispatch_plan"
''' name of the service plan dispatch
'''

cl_dispatch = None
''' (ros client handle) plan dispatcher
'''

topic_action_feedback = "/robocluedo/action_feedback"
''' the topic for receiving the feedback
'''

sub_action_feedback = None
''' (ros subscriber handle) the feedback subscription
'''

action_feedback_received = False
''' any pending feedback?
'''

action_feedback_msg = None
''' the pending feedback from the actions
'''

service_pipeline = "/robocluedo/pipeline_manager"
''' service for the pipeline command manager
'''

srv_pipeline = None
''' (ros service handle) service handle for the pipeline service
'''

service_update_goal = "/update_goal"
''' used to apply a landmark to the knowledge base
'''

cl_update_goal = None
''' (ros client handle) set a particular goal into the rosplan knowledge base
'''

def cbk_problem_instance( problem ): # std_msgs/String
	''' this callback received the problem instance from the problem interface
	'''
	
	global problem_instance_received
	global problem_instance_text
	
	problem_instance_received = True
	problem_instance_text = problem.data


def cbk_planning_interface( plan ): # std_msgs/String
	''' this callback receives the output from the planner, if any
	'''
	
	global planner_interface_received
	global planner_interface_text
	
	planner_interface_received = True
	planner_interface_text = plan.data


def cbk_parsing_interface( plan ): # rosplan_dispatch_msgs/CompletePlan
	''' this callback receives the output from the parsing interface, if any
	'''
	
	global parsing_interface_received
	global parsing_interface_array
	
	parsing_interface_received = True
	parsing_interface_array = plan


def cbk_action_feedback( feedback ):
	''' receive a feedback from the ROSPlan action
	'''
	
	global action_feedback_msg
	global action_feedback_received
	
	action_feedback_msg = feedback
	action_feedback_received = True


def inspect_planner_output( fpath ):
	''' the function reurns true when the problem has been found
		unsolvable. Otherwise, it returns false.
	'''
	if not os.path.exists( fpath ):
		return False
	
	fcontent = ""
	
	with open( fpath, "r" ) as fp:
		fcontent = fp.read( )
	
	return ( fcontent.find("the problem has been deemed unsolvable") < 0 )



def cbk_pipeline( req ):
	''' implementation of the service pipeline manager
	'''
	
	global cl_problem
	global problem_instance_received
	global problem_instance_text
	
	global cl_update_goal
	
	global cl_plan
	global planner_interface_received
	global planner_interface_text
	global planner_interface_solution_path
	
	global cl_parse
	global parsing_interface_received
	global parsing_interface_array
	
	global cl_dispatch
	global action_feedback_msg
	global action_feedback_received
	
	res = RosplanPipelineManagerServiceResponse( )
	res.success = True
	res.success_load_problem = True
	res.landmark_is_applicable = True
	res.success_solve_problem = True
	res.problem_not_solvable = False
	res.success_parse_plan = True
	res.success_execute_plan = True
	res.feedback_received = False
	
	
	## === PROBLEM INTERFACE === ## 
	
	if req.load_problem:
		# needed a landmark!
		if req.landmark < 0 or req.landmark > 2:
			rospy.logwarn(f"({NODE_NAME}) not a valid landmark: {req.landmark}")
			
			res.success_load_problem = False
			res.success = False
			return res
		else:
			rospy.loginfo(f"({NODE_NAME}) problem interface -- loading problem ...")
			
			# set the landmark into the ontology
			newgoal = UpdateGoalRequest( )
			newgoal.landmark = req.landmark
			# print(type(req.landmark))
			newgoal_res = cl_update_goal( newgoal )
			if not newgoal_res.success:
				
				if newgoal_res.applicable:
					rospy.logwarn(f"({NODE_NAME}) unable to apply the landmark {req.landmark} (reason: UNKNOWN)")
					
				else:
					rospy.logwarn(f"({NODE_NAME}) unable to apply the landmark {req.landmark} (reason: landmark not applicable)")
					res.landmark_is_applicable = False
				
				res.success_load_problem = False
				res.success = False
				return res
			else:
				rospy.loginfo(f"({NODE_NAME}) problem interface -- applied landmark {req.landmark}")
			
			problem_instance_text = ""
			problem_instance_received = False
			
			# trigger the problem generation and wait
			cl_problem( )
			rospy.sleep(rospy.Duration(1))
			
			if not problem_instance_received:
				rospy.logwarn(f"({NODE_NAME}) something went wrong while generating the problem instance")
				
				res.success = False
				res.success_load_problem = False
				return res
			else:
				rospy.loginfo(f"({NODE_NAME}) problem interface -- problem loading SUCCEEDED with landmark {req.landmark}")
	else:
		rospy.loginfo(f"({NODE_NAME}) problem interface -- skipping problem generation")
	
	
	## === PLANNER INTERFACE === ## 
	
	if req.solve_problem:
		rospy.loginfo(f"({NODE_NAME}) planning interface -- trying to solve ...")
		
		planner_interface_received = False
		planner_interface_text = ""
		exception_raised = False
		
		# trigger the planner
		try:
			cl_plan( )
			rospy.sleep(rospy.Duration(1))
		except rospy.ServiceException as serv_exc:
			rospy.logwarn(f"({NODE_NAME}) planning interface -- raised an exception ({serv_exc})")
			exception_raised = True
		
		if (not planner_interface_received) or exception_raised:
			rospy.logwarn(f"({NODE_NAME}) something went wrong in solving the problem")
			
			res.success = False
			res.success_solve_problem = False
			res.problem_not_solvable = inspect_planner_output( planner_interface_solution_path )
			
			if res.problem_not_solvable:
				rospy.logwarn(f"({NODE_NAME}) PROBLEM SEEMS UNSOLVABLE")
			
			return res
			
		else:
			rospy.loginfo(f"({NODE_NAME}) planning interface -- SOLVED")
		
	else:
		rospy.loginfo(f"({NODE_NAME}) planning interface -- skipping problem solution")
	
	
	## === PLANNER INTERFACE === ## 
	
	if req.parse_plan:
		rospy.loginfo(f"({NODE_NAME}) parsing interface -- parsing...")
		
		parsing_interface_received = False
		parsing_interface_array = []
		
		try:
			cl_parse( )
			rospy.sleep(rospy.Duration(1))
			
		except rospy.ServiceException:
			rospy.loginfo(f"({NODE_NAME}) parsing interface -- something went wrong while parsing the plan")
			
			res.success = False
			res.success_parse_plan = False
			return res
		
		if not parsing_interface_received:
			rospy.loginfo(f"({NODE_NAME}) parsing interface -- something went wrong while parsing the plan")
			
			res.success = False
			res.success_parse_plan = False
			return res
			
		else:
			rospy.loginfo(f"({NODE_NAME}) parsing interface -- plan parsing SUCCESS")
	else:
		rospy.loginfo(f"({NODE_NAME}) parsing interface -- skipping plan parsing")
	
	
	## === DISPATCH === ## 
	
	if req.execute_plan:
		rospy.loginfo(f"({NODE_NAME}) dispatch -- dispatching plan")
		
		action_feedback_received = False
		action_feedback_msg = None
		
		res_disp = None
		
		# trigger and wait
		try:
			res_disp = cl_dispatch()
			rospy.sleep(rospy.Duration(1))
		except rospy.serviceException as e:
			rospy.logwarn(f"({NODE_NAME}) dispatch -- ERROR in calling the dispatch service ({e})")
			
			res.success = False
			res.success_execute_plan = False
			res.feedback_received = False
			
			return res
		
		if not res_disp.goal_achieved:
			res.success = False
			res.success_execute_plan = False
			
			# need to look at the feedback
			if action_feedback_msg != None:
				res.feedback_received = True
				res.feedback = action_feedback_msg
				
				rospy.logwarn(f"({NODE_NAME}) dispatch -- FAILURE with feedback received")
			else:
				# unable to explain the failure
				rospy.logwarn(f"({NODE_NAME}) dispatch -- FAILURE with no feedback")
				
				res.feedback_received = False
			
			return res
		
		else:
			res.feedback_received = True
			res.feedback.goal_achieved = True
			res.feedback.details = "goal achieved"
		
	else:
		rospy.loginfo(f"({NODE_NAME}) dispatch -- skipping")
	
	return res





def shut_msg( ):
	''' called at the shutdown of the node, just a message
	'''
	rospy.loginfo( f"({NODE_NAME}) stopping ... " )


def open_cl( cl_name, cl_type ):
	''' handful utility to open a client
	'''
	
	global SRV_TIMEOUT
	global NODE_NAME
	
	rospy.loginfo(f"({NODE_NAME}) client: {cl_name} ... ")
	try:
		rospy.wait_for_service( cl_name, timeout=SRV_TIMEOUT )
		
	except rospy.ROSException as e:
		rospy.logwarn(f"({NODE_NAME}) client: {cl_name} UNABLE TO CONTACT within the timeout ({SRV_TIMEOUT}s) cause: {e}")
		raise e
		
	cl_this = rospy.ServiceProxy( cl_name, cl_type )
	rospy.loginfo(f"({NODE_NAME}) OK")
	
	rospy.sleep(rospy.Duration(0.75))
	
	return cl_this


if __name__ == "__main__":
	try:
		rospy.init_node( NODE_NAME )
		rospy.on_shutdown( shut_msg )
		
		rospy.loginfo(f"{NODE_NAME} starting... ")
		
		planner_interface_solution_path = rospy.get_param( "/output_problem_path", "" )
		if planner_interface_solution_path == "" :
			rospy.logwarn( f"({NODE_NAME}) plan output path missing! ('/output_problem_path' no defined in the parameter server)" )
		else:
			rospy.loginfo(f"({NODE_NAME}) planning output path: {planner_interface_solution_path} (removed before flight)")
			try:
				os.remove( planner_interface_solution_path )
			except FileNotFoundError:
				pass
		
		cl_problem = open_cl( service_problem, Empty )
		cl_plan = open_cl( service_plan, Empty )
		cl_parse = open_cl( service_parse, Empty )
		cl_dispatch = open_cl( service_dispatch, DispatchService )
		
		cl_update_goal = open_cl( service_update_goal, UpdateGoal )
		
		rospy.loginfo(f"({NODE_NAME}) service: {service_pipeline} ... ")
		srv_pipeline = rospy.Service( service_pipeline, RosplanPipelineManagerService, cbk_pipeline )
		rospy.sleep(rospy.Duration(0.75))
		rospy.loginfo(f"({NODE_NAME}) OK")
		
		rospy.loginfo(f"({NODE_NAME}) subscription: {topic_action_feedback} ... ")
		sub_action_feedback = rospy.Subscriber( topic_action_feedback, ActionFeedback, cbk_action_feedback )
		rospy.sleep(rospy.Duration(0.75))
		rospy.loginfo(f"({NODE_NAME}) OK")
		
		rospy.loginfo(f"({NODE_NAME}) subscription: {topic_problem_instance} ... ")
		sub_problem_instance = rospy.Subscriber( topic_problem_instance, String, cbk_problem_instance )
		rospy.sleep(rospy.Duration(0.75))
		rospy.loginfo(f"({NODE_NAME}) OK")
		
		rospy.loginfo(f"({NODE_NAME}) subscription: {topic_planner_interface} ... ")
		sub_planner_interface = rospy.Subscriber( topic_planner_interface, String, cbk_planning_interface )
		rospy.sleep(rospy.Duration(0.75))
		rospy.loginfo(f"({NODE_NAME}) OK")
		
		rospy.loginfo(f"({NODE_NAME}) subscription: {topic_parser_interface} ... ")
		sub_parser_interface = rospy.Subscriber( topic_parser_interface, CompletePlan, cbk_parsing_interface )
		rospy.sleep(rospy.Duration(0.75))
		rospy.loginfo(f"({NODE_NAME}) OK")
		
		rospy.loginfo(f"{NODE_NAME} ready")
		rospy.spin()
		
	except rospy.ROSException as e:
		rospy.logwarn("ROS Exception raised!")
		rospy.logwarn(f"exception text: {e}")
