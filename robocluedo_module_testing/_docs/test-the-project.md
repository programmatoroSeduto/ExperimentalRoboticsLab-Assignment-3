
# Testing the project

```{toctree}
./test-the-project.md
```

this document provides useful informations for testing the components of the architecture before running it entirely. 

*this document also contains the expected outputs from the programs*, or at least the outputs not too much long. 

## before proceeding

- be sure that *the entire project has been compiled entirely without errors*, see the instructions

## TESTING PKG robocluedo_armor

### pure test on aRMOR

this test lets to understand if aRMOR is doing its work in the correct way. 

in the package `robocluedo_module_Testing` there's a node called `test_armor.cpp` whose job is just to perform the most basic operations on aRMOR. *If even this simple script doesn't work, there's a problem with your installation of aRMOR*. 

**how to run the test:**

```bash
# shell 1
roslaunch robocluedo_module_testing test_armor.launch

# shell 2
rosrun robocluedo_module_testing test_armor

```

**NOTE** : this simple test will also create a owl file into the desktop.

### tesing aRMOR tools

the class `armor_tools` is the low-level set of services used by the armor interface to work. It provides very simple services, then made more detailed by its child class `armor_cluedo`. 

**how to run the test:**

```bash
# shell 1
roslaunch robocluedo_module_testing test_armor.launch

# shell 2
rosrun robocluedo_module_testing test_armor_tools

```

### Testing aRMOR cluedo

the class `armor_cluedo` takes into account the specific characteristics of the problem, making the interaction between the system and the ontology even easier.

**how to run the test:**

```bash
# shell 1
roslaunch robocluedo_module_testing test_armor.launch

# shell 2
rosrun robocluedo_module_testing test_armor_cluedo

```

## TESTING PKG robocluedo_rosplan

most of the packages here can be tested using simple scripts from a console. no special node is needed. 

### PDDL model

the model should work well, but to be sure, there's a simple way to test out it:

- go inside the folder *$(find robocluedo_rosplan)/pddl/robocluedo* (or better, use `roscd` to find rosplan): here you will find everything needed to test the model
- *without updating,* try to solve the projeb with `solve_popf.sh` : the solution should include only one action, `(replan )`
- open the file `robocluedo_problem.pddl` and replace the goal with this:
	
	```bash
	(:goal (and (dirty ) (hint-collected wp1 ) (hint-collected wp3 ) (at-center )))
	```
	
- then, `./solve_popf.sh` will return the entire plan. 
- and, finally, remember to restore the previous goal!
	
	```bash
	(:goal (not-dirty ))
	```

### simple ROSPlan pipeline

this test aims at testing how the architecture of ROSPlan works without running the pipeline manager nor the other components attached with the ontology. Just to be sure that ROSPlan is working well.

in this test, we're going to use the *RoboCLuedo PDDL model with the Simulated Actions*. Use the bash commands here below:

```bash
# shell 1
roslaunch robocluedo_module_testing load_rosplan_simulated_actions.launch

# shell 2
rosservice call /rosplan_problem_interface/problem_generation_server
rostopic echo /rosplan_problem_interface/problem_instance -n 1 -p
rosservice call /rosplan_planner_interface/planning_server
rostopic echo /rosplan_planner_interface/planner_output -p -n 1
rosservice call /rosplan_parsing_interface/parse_plan
rostopic echo /rosplan_parsing_interface/complete_plan -n 1
rosservice call /rosplan_plan_dispatcher/dispatch_plan "{}" 

```

### ROSPlan replanning and knowledge base update

the package robocluedo_rosplan has a fundamental node called `kb_interface` updating the knowledge according to the landmark. It provdes a fundamental funcitonality for performing the replanning, and its architecture is heavily specialized for working with this PDDL.

**Notice that** the main node to interact with when using ROSPlan is the pipeline manager, and not this node, *which should be hidden in the package*. 

```bash
# shell 1
roslaunch robocluedo_module_testing load_rosplan_simulated_actions.launch

# shell 2
rosrun robocluedo_rosplan kb_interface

# shell 3
rosservice list | grep update_goal
rosservice list | grep replan

# many warnings should appear on the ROSPlan console
rosservice call /replan "{}" 

rosservice call /update_goal "landmark: 0"
rosservice call /rosplan_problem_interface/problem_generation_server
rosservice call /rosplan_planner_interface/planning_server
rostopic echo /rosplan_planner_interface/planner_output -p -n 1
rosservice call /rosplan_parsing_interface/parse_plan
rosservice call /rosplan_plan_dispatcher/dispatch_plan "{}" 

# the robot will go in one wp
rosservice call /update_goal "landmark: 1"
rosservice call /rosplan_problem_interface/problem_generation_server
rosservice call /rosplan_planner_interface/planning_server
rostopic echo /rosplan_planner_interface/planner_output -p -n 1
rosservice call /rosplan_parsing_interface/parse_plan
rosservice call /rosplan_plan_dispatcher/dispatch_plan "{}" 

# if you repeat the test above, the wp gonna change (max 4 times)
# ...

# the fifth time the landmark is applied, the kb_interface
#    should reply with the flag "applicable=false"

# called the action "solve"
rosservice call /update_goal "landmark: 2"
rosservice call /rosplan_problem_interface/problem_generation_server
rosservice call /rosplan_planner_interface/planning_server
rostopic echo /rosplan_planner_interface/planner_output -p -n 1
rosservice call /rosplan_parsing_interface/parse_plan
rosservice call /rosplan_plan_dispatcher/dispatch_plan "{}" 

```

consider that, each time all the waypoints have been explored, the robot must call the service `/replan` to reset its status.

### ROSPlan pipeline manager with simulated actions

the main component controlling the ROSPlan architecture is the so called `rosplan_pipeline_manager`. It provides functionalities for doing many things:

- controlling each part of the planning process, *one by one*
- ease the interaction with the other parts of the system
- returning precide informations about the outcomes of the single pipeline components: indeed the basic ROSPlan architecture doesn't provide informations about the planning process *in a simple way*, and sometimes *the process to infer what happened inside could be cumbersome for the other parts of the architecture*. 

here's how to test it:

```text
# shell 1
roslaunch robocluedo_module_testing load_rosplan_simulated_actions.launch

# shell 2
rosrun robocluedo_rosplan kb_interface

# shell 3 (a bit slow, wait)
rosrun robocluedo_rosplan rosplan_pipeline_manager.py

# shell 4
rosservice list | grep robocluedo
#    /robocluedo/pipeline_manager

# skip everything
rosservice call /robocluedo/pipeline_manager "{load_problem: false, solve_problem: false, parse_plan: false, execute_plan: false, landmark: 0}" 
# loading only
rosservice call /robocluedo/pipeline_manager "{load_problem: true, solve_problem: false, parse_plan: false, execute_plan: false, landmark: 0}" 
# load + solving
rosservice call /robocluedo/pipeline_manager "{load_problem: true, solve_problem: true, parse_plan: false, execute_plan: false, landmark: 0}" 
# load + solving + parsing
rosservice call /robocluedo/pipeline_manager "{load_problem: true, solve_problem: true, parse_plan: true, execute_plan: false, landmark: 0}" 
# also dispatch
rosservice call /robocluedo/pipeline_manager "{load_problem: true, solve_problem: true, parse_plan: true, execute_plan: true, landmark: 0}" 

# landmarks
rosservice call /robocluedo/pipeline_manager "{load_problem: true, solve_problem: true, parse_plan: true, execute_plan: true, landmark: 0}" 
rosservice call /robocluedo/pipeline_manager "{load_problem: true, solve_problem: true, parse_plan: true, execute_plan: true, landmark: 1}" 
rosservice call /robocluedo/pipeline_manager "{load_problem: true, solve_problem: true, parse_plan: true, execute_plan: true, landmark: 2}" 
```

**Notice that** this sequence could ends to a *unsolvable problem* (it is due to the initial topology), and it makes sense because we're using just simulated actions:

```bash
rosservice call /robocluedo/pipeline_manager "{load_problem: true, solve_problem: true, parse_plan: true, execute_plan: true, landmark: 0}" 
rosservice call /robocluedo/pipeline_manager "{load_problem: true, solve_problem: true, parse_plan: true, execute_plan: true, landmark: 1}" 
rosservice call /robocluedo/pipeline_manager "{load_problem: true, solve_problem: true, parse_plan: true, execute_plan: true, landmark: 1}" 
rosservice call /robocluedo/pipeline_manager "{load_problem: true, solve_problem: true, parse_plan: true, execute_plan: true, landmark: 2}" 
rosservice call /robocluedo/pipeline_manager "{load_problem: true, solve_problem: true, parse_plan: true, execute_plan: true, landmark: 0}" 
# UNSOLVABLE
rosservice call /robocluedo/pipeline_manager "{load_problem: true, solve_problem: true, parse_plan: true, execute_plan: true, landmark: 1}" 
```

the topology that generated the unsolvable problem:

```lisp
(:init
	(dirty )
	
	(is-center center )
		(not-is-center wp1 ) (not-is-center wp2 ) 
		(not-is-center wp3 ) (not-is-center wp4 )
	
	(at center ) (at-center )
	
	(passage center wp1 ) (passage wp1 center )
	;;(passage center wp2 ) (passage wp2 center )
	;;(passage center wp3 ) (passage wp3 center )
	(passage center wp4 ) (passage wp4 center )
	(passage wp1 wp2 ) (passage wp2 wp3 ) (passage wp3 wp4 )
	
	(not-explored wp1 ) (not-explored wp2 ) (not-explored wp3 ) (not-explored wp4 )  
	(explored center )
	
	(not-hint-collected wp1 )
	(not-hint-collected wp2 )
	(not-hint-collected wp3 )
	(not-hint-collected wp4 )
	(not-hint-ready )
)
```

### ROSPlan pipeline manager with simulated actions -- NO LAUNCH FILE

```text
# shell 1
roslaunch robocluedo_rosplan load_rosplan.launch

# shell 2
rosrun robocluedo_rosplan kb_interface

# shell 3
rosrun robocluedo_module_testing rosplan_simulated_motion_system.py

# shell 4
rosrun robocluedo_rosplan rosplan_pipeline_manager.py

# shell 5
#     landmarks: 0 1 1 1 2 0 1 2

rosservice call /robocluedo/pipeline_manager "{load_problem: true, solve_problem: true, parse_plan: true, execute_plan: true, landmark: 0}" 
rosservice call /robocluedo/pipeline_manager "{load_problem: true, solve_problem: true, parse_plan: true, execute_plan: true, landmark: 1}" 
rosservice call /robocluedo/pipeline_manager "{load_problem: true, solve_problem: true, parse_plan: true, execute_plan: true, landmark: 1}" 
rosservice call /robocluedo/pipeline_manager "{load_problem: true, solve_problem: true, parse_plan: true, execute_plan: true, landmark: 1}" 
rosservice call /robocluedo/pipeline_manager "{load_problem: true, solve_problem: true, parse_plan: true, execute_plan: true, landmark: 2}"

# ERRORE problema non risolvibile (correttamente segnalato dal pipeline manager)
rosservice call /robocluedo/pipeline_manager "{load_problem: true, solve_problem: true, parse_plan: true, execute_plan: true, landmark: 1}" 

# problema risolvibile
rosservice call /robocluedo/pipeline_manager "{load_problem: true, solve_problem: true, parse_plan: true, execute_plan: true, landmark: 0}" 
rosservice call /robocluedo/pipeline_manager "{load_problem: true, solve_problem: true, parse_plan: true, execute_plan: true, landmark: 1}" 
rosservice call /robocluedo/pipeline_manager "{load_problem: true, solve_problem: true, parse_plan: true, execute_plan: true, landmark: 1}" 
rosservice call /robocluedo/pipeline_manager "{load_problem: true, solve_problem: true, parse_plan: true, execute_plan: true, landmark: 2}"

```

### the same test with launch file

```bash
# shell 1
roslaunch robocluedo_rosplan run.launch

# shell 2
rosrun robocluedo_module_testing rosplan_simulated_motion_system.py

# shell 3
rosservice call /robocluedo/pipeline_manager "{load_problem: true, solve_problem: true, parse_plan: true, execute_plan: true, landmark: 0}" 
rosservice call /robocluedo/pipeline_manager "{load_problem: true, solve_problem: true, parse_plan: true, execute_plan: true, landmark: 1}" 
rosservice call /robocluedo/pipeline_manager "{load_problem: true, solve_problem: true, parse_plan: true, execute_plan: true, landmark: 1}" 
rosservice call /robocluedo/pipeline_manager "{load_problem: true, solve_problem: true, parse_plan: true, execute_plan: true, landmark: 1}" 
rosservice call /robocluedo/pipeline_manager "{load_problem: true, solve_problem: true, parse_plan: true, execute_plan: true, landmark: 2}"

# ERRORE problema non risolvibile (correttamente segnalato dal pipeline manager)
rosservice call /robocluedo/pipeline_manager "{load_problem: true, solve_problem: true, parse_plan: true, execute_plan: true, landmark: 1}" 

# problema risolvibile
rosservice call /robocluedo/pipeline_manager "{load_problem: true, solve_problem: true, parse_plan: true, execute_plan: true, landmark: 0}" 
rosservice call /robocluedo/pipeline_manager "{load_problem: true, solve_problem: true, parse_plan: true, execute_plan: true, landmark: 1}" 
rosservice call /robocluedo/pipeline_manager "{load_problem: true, solve_problem: true, parse_plan: true, execute_plan: true, landmark: 1}" 
rosservice call /robocluedo/pipeline_manager "{load_problem: true, solve_problem: true, parse_plan: true, execute_plan: true, landmark: 2}"

```

## TESTING PKG robocluedo_robot

### BEFORE THE TESTS...

make sure to have the updated version of the model. 

- shell: `roscd robocluedo_urdf_model` and `cd robot`
- run the script `./generate_mode.sh` : the model should appear as console output

### the most simple test!

*try and guess?* let's try to ... execute the main launch file. You should not see something strange ... or, worse, errors or crashes. 

**first test:** Gazebo only

```bash
roslaunch robocluedo_robot gazebo.launch

```

**second test:** RViz only

```bash
roslaunch robocluedo_robot demo.launch

```

**Third test:** Gazebo and RViz (it should generate loads of warnings on the console ... )

```bash
roslaunch robocluedo_robot demo_gazebo.launch

```

the same launcher, without warnings (not necessairly a good thing...):

```bash
roslaunch robocluedo_robot demo_gazebo.launch 2>/dev/null

```

## TESTING PKG robocluedo_movement_controller

the package contains nodes for performing the navigation and the manipulation. 

### go to opint

this py node performs a simple navigation between two points in a path without obstacles.

```bash
# shell 1
roslaunch robocluedo_robot gazebo.launch 2>/dev/null

# shell 2 (it takes minimum 4 seconds to start)
rosparam set des_pos_x 0.0
rosparam set des_pos_y 0.0
rosrun robocluedo_movement_controller go_to_point_service_m.py

# shell 3
rosparam set des_pos_x 2.0
rosparam set des_pos_y 2.0

#no responding from this
rosservice call /go_to_point_switch "data: false" 

# the robot starts moving
rosservice call /go_to_point_switch "data: true" 

# a more structured test
rosservice call /go_to_point_switch "data: false" 
rosparam set des_pos_x -2.0
rosparam set des_pos_y -2.0
rosservice call /go_to_point_switch "data: true" 
sleep 20
rosservice call /go_to_point_switch "data: false" 
rosparam set des_pos_x -2.0
rosparam set des_pos_y 2.0
rosservice call /go_to_point_switch "data: true" 
sleep 20
rosparam set des_pos_x 2.0
rosparam set des_pos_y -2.0
rosservice call /go_to_point_switch "data: true" 
sleep 20
rosparam set des_pos_x 2.0
rosparam set des_pos_y 2.0
rosservice call /go_to_point_switch "data: true" 
sleep 20
rosservice call /go_to_point_switch "data: false" 

```

### head orientation

this node simply rotates the robot of a certain value of radiants. 

```bash
# shell 1
roslaunch robocluedo_robot gazebo.launch 2>/dev/null

# shell 2 (it takes minimum 4 seconds to start)
rosparam set des_yaw 0.0
rosrun robocluedo_movement_controller head_orientation.py

# shell 3
rosservice call /head_orient_switch "data: false"
rosparam set des_yaw 2.0
rosservice call /head_orient_switch "data: true"
sleep 15
rosservice call /head_orient_switch "data: false"
rosparam set des_yaw -0.5
rosservice call /head_orient_switch "data: true"
sleep 15
rosservice call /head_orient_switch "data: false"
rosparam set des_yaw 1.57
rosservice call /head_orient_switch "data: true"
sleep 15
rosservice call /head_orient_switch "data: false"
rosparam set des_yaw -1.57
rosservice call /head_orient_switch "data: true"
sleep 15
rosservice call /head_orient_switch "data: false"
rosparam set des_yaw 3.13
rosservice call /head_orient_switch "data: true"
sleep 15

```

### wall follow behaviour

```bash
# shell 1
roslaunch robocluedo_robot gazebo.launch 2>/dev/null

# shell 2 (it takes minimum 4 seconds to start)
rosrun robocluedo_movement_controller wall_follow_service_m.py

# shell 3
rosservice call /wall_follower_switch "data: true"
sleep 30
rosservice call /wall_follower_switch "data: false"

```

### the bug_m module

the so called bug_m is a navigation controller made up of four nodes:

- `bug_m` is the main node
- `go_to_point_service_m` , `head_orientation` and `wall_follow_service_m`

here's the test:

```bash
# shell 1
roslaunch robocluedo_robot gazebo.launch

# shell 2
rosparam set des_pos_x 0.0
rosparam set des_pos_y 0.0
rosparam set des_yaw 0.0
rosrun robocluedo_movement_controller wall_follow_service_m.py &
rosrun robocluedo_movement_controller go_to_point_service_m.py &
rosrun robocluedo_movement_controller head_orientation.py & 
rosrun robocluedo_movement_controller bug_m.py

# shell 3
# rosservice list | grep switch

rosservice call /bug_switch "data: false" 
rosparam set des_pos_x 2.0
rosparam set des_pos_y 0.0
rosparam set des_yaw 0.0
rosservice call /bug_switch "data: true" 
sleep 15
rosservice call /bug_switch "data: false" 
rosparam set des_pos_x 0.0
rosparam set des_pos_y 2.0
rosparam set des_yaw 1.57
rosservice call /bug_switch "data: true" 
sleep 15
rosservice call /bug_switch "data: false" 
rosparam set des_pos_x -2.0
rosparam set des_pos_y 0.0
rosparam set des_yaw 3.14
rosservice call /bug_switch "data: true" 
sleep 15
rosservice call /bug_switch "data: false" 
rosparam set des_pos_x 0.0
rosparam set des_pos_y -2.0
rosparam set des_yaw -1.57
rosservice call /bug_switch "data: true" 
sleep 15
rosservice call /bug_switch "data: false" 

```

### testing navigation manager

the so called node *navigation manager* allows

- to interact with the navigation system making an abstraction between the navigation command and the real component implementing the navigation
- to change at runtine navigation component in order to perform different kinds of movement depending on the situation


```bash
# shell 1
roslaunch robocluedo_robot gazebo.launch 2>/dev/null

# shell 2
roslaunch robocluedo_movement_controller navigation_system.launch

# shell 3
rosrun robocluedo_movement_controller navigation_manager

#shell 4

# this sequence produces a error (it is right!)
rosservice call /navigation_manager/set_algorithm "algorithm: 0
enabled: false"

rosservice call /navigation_manager/navigation "target:
  x: 1.5
  y: 2.0
  yaw: 1.0
force_enable: false" 

# this sequence moves the robot
rosservice call /navigation_manager/set_algorithm "algorithm: 0
enabled: true"

rosservice call /navigation_manager/navigation "target:
  x: -1.5
  y: 2.0
  yaw: 1.0
force_enable: false" 

# this sequence moves the robot EVEN IF the component has been turned off
rosservice call /navigation_manager/set_algorithm "algorithm: 0
enabled: false"

rosservice call /navigation_manager/navigation "target:
  x: 1.5
  y: -2.0
  yaw: 1.0
force_enable: true" 

```

### manipulation controller

```bash
# shell 1
roslaunch robocluedo_robot demo_gazebo.launch 2>/dev/null

# shell 2
rosrun robocluedo_movement_controller manipulation_controller 2>/dev/null

# shell 3

rosservice list | grep tip
# /tip_pos

rosservice call /tip_pos "command: 0" 
sleep 2
rosservice call /tip_pos "command: 1" 
sleep 2
rosservice call /tip_pos "command: 2" 
sleep 2
rosservice call /tip_pos "command: 3" 
sleep 2
rosservice call /tip_pos "command: 4" 
sleep 2

# UNKNOWN POSE
rosservice call /tip_pos "command: 5" 
sleep 2

# NOT A VALID POSE
rosservice call /tip_pos "command: 5" 
sleep 2

```

## TESTING rosplan plus real motion controller

it is the same last test of robocluedo ROSPlan, but this time using the real movement controller. 

```bash
# shell 1
roslaunch robocluedo_mission_manager run_components.launch 2>/dev/null

# shell 2
rostopic echo /oracle_hint

# shell 2
rosservice call /robocluedo/pipeline_manager "{load_problem: true, solve_problem: true, parse_plan: true, execute_plan: true, landmark: 0}" 
rosservice call /robocluedo/pipeline_manager "{load_problem: true, solve_problem: true, parse_plan: true, execute_plan: true, landmark: 1}" 
rosservice call /robocluedo/pipeline_manager "{load_problem: true, solve_problem: true, parse_plan: true, execute_plan: true, landmark: 1}" 
rosservice call /robocluedo/pipeline_manager "{load_problem: true, solve_problem: true, parse_plan: true, execute_plan: true, landmark: 1}" 
rosservice call /robocluedo/pipeline_manager "{load_problem: true, solve_problem: true, parse_plan: true, execute_plan: true, landmark: 2}"

rosservice call /robocluedo/pipeline_manager "{load_problem: true, solve_problem: true, parse_plan: true, execute_plan: true, landmark: 0}" 
rosservice call /robocluedo/pipeline_manager "{load_problem: true, solve_problem: true, parse_plan: true, execute_plan: true, landmark: 1}" 
rosservice call /robocluedo/pipeline_manager "{load_problem: true, solve_problem: true, parse_plan: true, execute_plan: true, landmark: 1}" 
rosservice call /robocluedo/pipeline_manager "{load_problem: true, solve_problem: true, parse_plan: true, execute_plan: true, landmark: 2}"

```

## Final Tests

**before running these tests** : 

- (optional) enable the macros `WAITKEY` in the `robocluedo_mission_manager/mission_manager.cpp` by setting `#define WAITKEY_ENABLED true`
	
	the program will stop the execution at each important step
	
- enable the development mode in the oracle located at `robocluedo_dependencied_erl2/src/simulation.cpp` : set `#define NOT_TESTING false`
	
	this option says the Oracle to send just the elements of th winner message and not all the other items. keys are selected randomly from the fields of the winner ID. 

### first test

this test allows to see the output from aRMOR.

```bash
# shell 1
roslaunch robocluedo_mission_manager run_components.launch 2>/dev/null

# shell 2
roslaunch robocluedo_armor run.launch

# shell 3
rosrun robocluedo_mission_manager mission_manager
```

### another test

this test is used for checking the behaviour of ROSPlan, and in particular of the pipeline manager (which is written in Python unfortunately).

*this test corresponds to how to run the project.*

```bash
# shell 1
roslaunch robocluedo_mission_manager run_components_2.launch 2>/dev/null

# shell 2
roslaunch robocluedo_rosplan run.launch

# shell 3
rosrun robocluedo_mission_manager mission_manager
```

## TESTS OUTPUTS

### armor only

**SHELL 1:**

```text
started roslaunch server http://3b17871017fd:43585/

SUMMARY
========

PARAMETERS
 * /cluedo_max_hypotheses: 15
 * /cluedo_path_owlfile: /root/ros_ws/src/...
 * /cluedo_path_owlfile_backup: /root/ros_ws/src/...
 * /cluedo_path_what: /root/ros_ws/src/...
 * /cluedo_path_where: /root/ros_ws/src/...
 * /cluedo_path_who: /root/ros_ws/src/...
 * /rosdistro: noetic
 * /rosversion: 1.15.14

NODES
  /
    armor_service (armor/execute)

auto-starting new master
process[master]: started with pid [1212]
ROS_MASTER_URI=http://localhost:11311

setting /run_id to 7984909e-1b00-11ed-af07-0242ac110002
process[rosout-1]: started with pid [1237]
started core service [/rosout]
process[armor_service-2]: started with pid [1240]
Loading node class: it.emarolab.armor.ARMORMainService
log4j:WARN No appenders could be found for logger (org.ros.internal.node.client.Registrar).
log4j:WARN Please initialize the log4j system properly.
log4j:WARN See http://logging.apache.org/log4j/1.2/faq.html#noconfig for more info.

```

**TEST OUTPUT:**

```text
root@3b17871017fd:~/ros_ws/src/erl2-new/robocluedo_module_testing/launch/test_armor# rosrun robocluedo_module_testing test_armor
[ INFO] [1660392484.885474700]: [test_armor] Ontology found! [/root/ros_ws/src/erl2-new/robocluedo_armor/config/cluedo_owl_ontology/cluedo_ontology.owl] 
[ INFO] [1660392484.886339200]: [test_armor] Requiring client [/armor_interface_srv] ...
[ INFO] [1660392484.886751600]: [test_armor] Requiring client [/armor_interface_srv] ... OK
[ INFO] [1660392484.886820800]: [test_armor] Requiring client [/armor_interface_serialized_srv] ...
[ INFO] [1660392484.887235800]: [test_armor] Requiring client [/armor_interface_serialized_srv] ... OK
[ INFO] [1660392484.887399800]: [test_armor] Print Request: 
	client_name : robocluedo
	reference_name : cluedo_ontology
	command : LOAD
	primary_command_spec : FILE
	secondary_command_spec : 
	args : [/root/ros_ws/src/erl2-new/robocluedo_armor/config/cluedo_owl_ontology/cluedo_ontology.owl http://www.emarolab.it/cluedo-ontology false PELLET true ]
	---

[ INFO] [1660392485.926716200]: [test_armor] Print Response: 
	success : true
	timeout : false
	exit_code : 0
	error_description : 
	is_consistent : true
	queried_objects :
	sparql_queried_objects : 
	---
[ INFO] [1660392485.926796200]: [test_armor] Print Request: 
	client_name : robocluedo
	reference_name : cluedo_ontology
	command : ADD
	primary_command_spec : IND
	secondary_command_spec : CLASS
	args : [Jim PERSON    ]
	---
[ INFO] [1660392485.944448400]: [test_armor] Print Response: 
	success : true
	timeout : false
	exit_code : 0
	error_description : 
	is_consistent : true
	queried_objects :
	sparql_queried_objects : 
	---
[ INFO] [1660392485.944523700]: [test_armor] Print Request: 
	client_name : robocluedo
	reference_name : cluedo_ontology
	command : ADD
	primary_command_spec : OBJECTPROP
	secondary_command_spec : IND
	args : [who HP3 Jim   ]
	---
[ INFO] [1660392485.955545300]: [test_armor] Print Response: 
	success : true
	timeout : false
	exit_code : 0
	error_description : 
	is_consistent : true
	queried_objects :
	sparql_queried_objects : 
	---
[ INFO] [1660392485.955632300]: [test_armor] Print Request: 
	client_name : robocluedo
	reference_name : cluedo_ontology
	command : ADD
	primary_command_spec : IND
	secondary_command_spec : CLASS
	args : [Gun WEAPON    ]
	---
[ INFO] [1660392485.962069000]: [test_armor] Print Response: 
	success : true
	timeout : false
	exit_code : 0
	error_description : 
	is_consistent : true
	queried_objects :
	sparql_queried_objects : 
	---
[ INFO] [1660392485.962144700]: [test_armor] Print Request: 
	client_name : robocluedo
	reference_name : cluedo_ontology
	command : ADD
	primary_command_spec : OBJECTPROP
	secondary_command_spec : IND
	args : [what HP3 Gun   ]
	---
[ INFO] [1660392485.967811800]: [test_armor] Print Response: 
	success : true
	timeout : false
	exit_code : 0
	error_description : 
	is_consistent : true
	queried_objects :
	sparql_queried_objects : 
	---
[ INFO] [1660392485.967919500]: [test_armor] Print Request: 
	client_name : robocluedo
	reference_name : cluedo_ontology
	command : ADD
	primary_command_spec : IND
	secondary_command_spec : CLASS
	args : [Stadium PLACE    ]
	---
[ INFO] [1660392485.973594300]: [test_armor] Print Response: 
	success : true
	timeout : false
	exit_code : 0
	error_description : 
	is_consistent : true
	queried_objects :
	sparql_queried_objects : 
	---
[ INFO] [1660392485.973706100]: [test_armor] Print Request: 
	client_name : robocluedo
	reference_name : cluedo_ontology
	command : ADD
	primary_command_spec : OBJECTPROP
	secondary_command_spec : IND
	args : [where HP3 Stadium   ]
	---
[ INFO] [1660392485.981074500]: [test_armor] Print Response: 
	success : true
	timeout : false
	exit_code : 0
	error_description : 
	is_consistent : true
	queried_objects :
	sparql_queried_objects : 
	---
[ INFO] [1660392485.981191700]: [test_armor] Print Request: 
	client_name : robocluedo
	reference_name : cluedo_ontology
	command : REASON
	primary_command_spec : 
	secondary_command_spec : 
	args : [     ]
	---
[ INFO] [1660392485.992977900]: [test_armor] Print Response: 
	success : true
	timeout : false
	exit_code : 0
	error_description : 
	is_consistent : true
	queried_objects :
	sparql_queried_objects : 
	---
[ INFO] [1660392485.993068900]: [test_armor] Print Request: 
	client_name : robocluedo
	reference_name : cluedo_ontology
	command : ADD
	primary_command_spec : IND
	secondary_command_spec : CLASS
	args : [Lounge PLACE    ]
	---
[ INFO] [1660392485.998666300]: [test_armor] Print Response: 
	success : true
	timeout : false
	exit_code : 0
	error_description : 
	is_consistent : true
	queried_objects :
	sparql_queried_objects : 
	---
[ INFO] [1660392485.998737200]: [test_armor] Print Request: 
	client_name : robocluedo
	reference_name : cluedo_ontology
	command : DISJOINT
	primary_command_spec : IND
	secondary_command_spec : 
	args : [Lounge Stadium    ]
	---
[ INFO] [1660392486.015745500]: [test_armor] Print Response: 
	success : true
	timeout : false
	exit_code : 0
	error_description : 
	is_consistent : true
	queried_objects :
	sparql_queried_objects : 
	---
[ INFO] [1660392486.015854100]: [test_armor] Print Request: 
	client_name : robocluedo
	reference_name : cluedo_ontology
	command : ADD
	primary_command_spec : OBJECTPROP
	secondary_command_spec : IND
	args : [where HP3 Lounge   ]
	---
[ INFO] [1660392486.021397900]: [test_armor] Print Response: 
	success : true
	timeout : false
	exit_code : 0
	error_description : 
	is_consistent : true
	queried_objects :
	sparql_queried_objects : 
	---
[ INFO] [1660392486.021513800]: [test_armor] Print Request: 
	client_name : robocluedo
	reference_name : cluedo_ontology
	command : REASON
	primary_command_spec : 
	secondary_command_spec : 
	args : [     ]
	---
[ INFO] [1660392486.039751300]: [test_armor] Print Response: 
	success : true
	timeout : false
	exit_code : 0
	error_description : 
	is_consistent : true
	queried_objects :
	sparql_queried_objects : 
	---
[ INFO] [1660392486.039833600]: [test_armor] Print Request: 
	client_name : robocluedo
	reference_name : cluedo_ontology
	command : SAVE
	primary_command_spec : INFERENCE
	secondary_command_spec : 
	args : [/root/Desktop/ROBOCLUEDO_ONTOLOGY.owl     ]
	---
[ INFO] [1660392486.200656300]: [test_armor] Print Response: 
	success : true
	timeout : false
	exit_code : 0
	error_description : 
	is_consistent : true
	queried_objects :
	sparql_queried_objects : 
	---
[ INFO] [1660392486.200766100]: [test_armor] closing...

```

### armor with armor_tools 

**SHELL 1:**

```text
root@3b17871017fd:~/ros_ws/src/erl2-new/robocluedo_module_testing/launch/test_armor# roslaunch robocluedo_module_testing test_armor.launch
... logging to /root/.ros/log/0cf4f092-1b03-11ed-ad08-0242ac110002/roslaunch-3b17871017fd-15182.log
Checking log directory for disk usage. This may take a while.
Press Ctrl-C to interrupt
Done checking log file disk usage. Usage is <1GB.

started roslaunch server http://3b17871017fd:32951/

SUMMARY
========

PARAMETERS
 * /cluedo_max_hypotheses: 15
 * /cluedo_path_owlfile: /root/ros_ws/src/...
 * /cluedo_path_owlfile_backup: /root/ros_ws/src/...
 * /cluedo_path_what: /root/ros_ws/src/...
 * /cluedo_path_where: /root/ros_ws/src/...
 * /cluedo_path_who: /root/ros_ws/src/...
 * /rosdistro: noetic
 * /rosversion: 1.15.14

NODES
  /
    armor_service (armor/execute)

auto-starting new master
process[master]: started with pid [15220]
ROS_MASTER_URI=http://localhost:11311

setting /run_id to 0cf4f092-1b03-11ed-ad08-0242ac110002
process[rosout-1]: started with pid [15245]
started core service [/rosout]
process[armor_service-2]: started with pid [15252]
Loading node class: it.emarolab.armor.ARMORMainService
log4j:WARN No appenders could be found for logger (org.ros.internal.node.client.Registrar).
log4j:WARN Please initialize the log4j system properly.
log4j:WARN See http://logging.apache.org/log4j/1.2/faq.html#noconfig for more info.



```

**SHELL 2:**

```text
root@3b17871017fd:~/ros_ws/src/erl2-new/robocluedo_module_testing/launch/test_armor# rosrun robocluedo_module_testing test_armor_tools
[ INFO] [1660394502.533233500]: [test_armor_tools] Ontology found! [/root/ros_ws/src/erl2-new/robocluedo_armor/config/cluedo_owl_ontology/cluedo_ontology.owl] 
[ INFO] [1660394502.533945000]: [armor_tools] Requiring client [/armor_interface_srv] ...
[ INFO] [1660394502.534393900]: [armor_tools] -> OK
[ INFO] [1660394503.535600400]: [test_armor_tools] (1) completed hypotheses:
[ INFO] [1660394503.535726200]:    Print Response: 
	success : true
	timeout : false
	exit_code : 0
	error_description : 
	is_consistent : true
	queried_objects :
	-	<http://www.emarolab.it/cluedo-ontology#HP3>
	sparql_queried_objects : 
	---
[ INFO] [1660394503.541773600]: [test_armor_tools] (1) inconsistent hypotheses:
[ INFO] [1660394503.541849400]:    Print Response: 
	success : true
	timeout : false
	exit_code : 0
	error_description : 
	is_consistent : true
	queried_objects :
	sparql_queried_objects : 
	---
[ INFO] [1660394503.560990000]:    Print Request: 
	client_name : armor_client
	reference_name : cluedo
	command : DISJOINT
	primary_command_spec : IND
	secondary_command_spec : CLASS
	args : [     ]
	---
[ INFO] [1660394503.561065200]:    Print Response: 
	success : true
	timeout : false
	exit_code : 0
	error_description : 
	is_consistent : true
	queried_objects :
	sparql_queried_objects : 
	---
[ INFO] [1660394503.578151000]: [test_armor_tools] (2) completed hypotheses:
[ INFO] [1660394503.591825200]:    Print Response: 
	success : true
	timeout : false
	exit_code : 0
	error_description : 
	is_consistent : true
	queried_objects :
	-	<http://www.emarolab.it/cluedo-ontology#HP3>
	sparql_queried_objects : 
	---
[ INFO] [1660394503.591923700]: [test_armor_tools] (2) inconsistent hypotheses:
[ INFO] [1660394503.597800300]:    Print Response: 
	success : true
	timeout : false
	exit_code : 0
	error_description : 
	is_consistent : true
	queried_objects :
	sparql_queried_objects : 
	---
[ INFO] [1660394503.610336500]: [test_armor_tools] (3) un disjoint strano:
[ INFO] [1660394503.610436400]:    Print Response: 
	success : true
	timeout : false
	exit_code : 0
	error_description : 
	is_consistent : true
	queried_objects :
	sparql_queried_objects : 
	---
[ INFO] [1660394503.610522800]: [test_armor_tools] (3) inconsistent hypotheses:
[ INFO] [1660394503.618661100]:    Print Response: 
	success : true
	timeout : false
	exit_code : 0
	error_description : 
	is_consistent : true
	queried_objects :
	sparql_queried_objects : 
	---
[ INFO] [1660394503.618737000]: [test_armor_tools] query elementi della classe PLACE
[ INFO] [1660394503.637675800]:    Print Response: 
	success : true
	timeout : false
	exit_code : 0
	error_description : 
	is_consistent : true
	queried_objects :
	-	<http://www.emarolab.it/cluedo-ontology#Study>
	-	<http://www.emarolab.it/cluedo-ontology#Stadium>
	-	<http://www.emarolab.it/cluedo-ontology#Lounge>
	sparql_queried_objects : 
	---
[ INFO] [1660394503.637773600]: [test_armor_tools] query proprieta' di hp3
[ INFO] [1660394503.671939100]:    Print Response: 
	success : true
	timeout : false
	exit_code : 0
	error_description : 
	is_consistent : true
	queried_objects :
	-	<http://www.emarolab.it/cluedo-ontology#what>
	-	<http://www.emarolab.it/cluedo-ontology#who>
	-	<http://www.emarolab.it/cluedo-ontology#where>
	sparql_queried_objects : 
	---
[ INFO] [1660394503.672028100]: [test_armor_tools] query di tutte le ipotesi
[ INFO] [1660394503.686505400]:    Print Response: 
	success : true
	timeout : false
	exit_code : 0
	error_description : 
	is_consistent : true
	queried_objects :
	-	<http://www.emarolab.it/cluedo-ontology#Jim>
	-	<http://www.emarolab.it/cluedo-ontology#>
	-	<http://www.emarolab.it/cluedo-ontology#Study>
	-	<http://www.emarolab.it/cluedo-ontology#Gun>
	-	<http://www.emarolab.it/cluedo-ontology#Stadium>
	-	<http://www.emarolab.it/cluedo-ontology#Lounge>
	-	<http://www.emarolab.it/cluedo-ontology#HP3>
	sparql_queried_objects : 
	---
[ INFO] [1660394503.686635900]: [test_armor_tools] query tutte le classi a cui HP3 apartiene
[ INFO] [1660394503.714323400]:    Print Response: 
	success : true
	timeout : false
	exit_code : 0
	error_description : 
	is_consistent : true
	queried_objects :
	-	<http://www.emarolab.it/cluedo-ontology#COMPLETED>
	-	<http://www.emarolab.it/cluedo-ontology#HYPOTHESIS>
	sparql_queried_objects : 
	---
[ INFO] [1660394503.714410400]: [test_armor_tools] salvataggio
[ INFO] [1660394503.832474000]: [test_armor_tools] THAT'S ALL FOLKS!

```

### armor with armor_cluedo and armor_tools

**SHELL 1:**

```text
root@3b17871017fd:~/ros_ws/src/erl2-new/robocluedo_module_testing/launch/test_armor# roslaunch robocluedo_module_testing test_armor.launch
... logging to /root/.ros/log/c81a496a-1b05-11ed-9d7f-0242ac110002/roslaunch-3b17871017fd-27615.log
Checking log directory for disk usage. This may take a while.
Press Ctrl-C to interrupt
Done checking log file disk usage. Usage is <1GB.

started roslaunch server http://3b17871017fd:43779/

SUMMARY
========

PARAMETERS
 * /cluedo_max_hypotheses: 15
 * /cluedo_path_owlfile: /root/ros_ws/src/...
 * /cluedo_path_owlfile_backup: /root/ros_ws/src/...
 * /cluedo_path_what: /root/ros_ws/src/...
 * /cluedo_path_where: /root/ros_ws/src/...
 * /cluedo_path_who: /root/ros_ws/src/...
 * /rosdistro: noetic
 * /rosversion: 1.15.14

NODES
  /
    armor_service (armor/execute)

auto-starting new master
process[master]: started with pid [27653]
ROS_MASTER_URI=http://localhost:11311

setting /run_id to c81a496a-1b05-11ed-9d7f-0242ac110002
process[rosout-1]: started with pid [27678]
started core service [/rosout]
process[armor_service-2]: started with pid [27685]
Loading node class: it.emarolab.armor.ARMORMainService
log4j:WARN No appenders could be found for logger (org.ros.internal.node.client.Registrar).
log4j:WARN Please initialize the log4j system properly.
log4j:WARN See http://logging.apache.org/log4j/1.2/faq.html#noconfig for more info.



```

**SHELL 2:**

```text
root@3b17871017fd:~/ros_ws/src/erl2-new/robocluedo_module_testing/launch/test_armor# rosrun robocluedo_module_testing test_armor_cluedo
[ INFO] [1660394741.152181200]: [test_armor_tools] Ontology found! [/root/ros_ws/src/erl2-new/robocluedo_armor/config/cluedo_owl_ontology/cluedo_ontology.owl] 
[ INFO] [1660394741.152996900]: [armor_tools] Requiring client [/armor_interface_srv] ...
[ INFO] [1660394741.153395100]: [armor_tools] -> OK
[ INFO] [1660394742.072103100]: [test_armor_tools] 


 TEST COMANDI DIRETTI ARMOR 



[ INFO] [1660394742.160877900]: [test_armor_tools] (0) completed hypotheses: (dovrebbe essere vuoto)
[ INFO] [1660394742.160996300]:    Print Response: 
	success : true
	timeout : false
	exit_code : 0
	error_description : 
	is_consistent : true
	queried_objects :
	sparql_queried_objects : 
	---
[ INFO] [1660394742.170764500]: [test_armor_tools] (0) inconsistent hypotheses: (dovrebbe essere vuoto)
[ INFO] [1660394742.170856100]:    Print Response: 
	success : true
	timeout : false
	exit_code : 0
	error_description : 
	is_consistent : true
	queried_objects :
	sparql_queried_objects : 
	---
[ INFO] [1660394742.228453400]: [test_armor_tools] (1) completed hypotheses: (dovrebbe comparire un'ipotesi)
[ INFO] [1660394742.228533900]:    Print Response: 
	success : true
	timeout : false
	exit_code : 0
	error_description : 
	is_consistent : true
	queried_objects :
	-	<http://www.emarolab.it/cluedo-ontology#HP3>
	sparql_queried_objects : 
	---
[ INFO] [1660394742.233656100]: [test_armor_tools] (1) inconsistent hypotheses: (la lista dovrebbe essere vuota)
[ INFO] [1660394742.233725800]:    Print Response: 
	success : true
	timeout : false
	exit_code : 0
	error_description : 
	is_consistent : true
	queried_objects :
	sparql_queried_objects : 
	---
[ INFO] [1660394742.317583600]: [test_armor_tools] (2) completed hypotheses: (sempre una sola ipotesi)
[ INFO] [1660394742.317663000]:    Print Response: 
	success : true
	timeout : false
	exit_code : 0
	error_description : 
	is_consistent : true
	queried_objects :
	-	<http://www.emarolab.it/cluedo-ontology#HP3>
	sparql_queried_objects : 
	---
[ INFO] [1660394742.325069700]: [test_armor_tools] (2) inconsistent hypotheses: (dovrebbe spuntare un'ipotesi inconsistente)
[ INFO] [1660394742.325156500]:    Print Response: 
	success : true
	timeout : false
	exit_code : 0
	error_description : 
	is_consistent : true
	queried_objects :
	-	<http://www.emarolab.it/cluedo-ontology#HP3>
	sparql_queried_objects : 
	---
[ INFO] [1660394742.333623100]: [test_armor_tools] proprieta' where dell'ipotesi HP3:
[ INFO] [1660394742.333695300]:    Print Response: 
	success : true
	timeout : false
	exit_code : 0
	error_description : 
	is_consistent : true
	queried_objects :
	-	<http://www.emarolab.it/cluedo-ontology#Study>
	-	<http://www.emarolab.it/cluedo-ontology#Stadium>
	sparql_queried_objects : 
	---
[ INFO] [1660394742.333792700]: [test_armor_tools] test esistenza di HP3:
[ INFO] [1660394742.334728000]:    Print Request: 
	client_name : armor_client
	reference_name : cluedo
	command : QUERY
	primary_command_spec : IND
	secondary_command_spec : 
	args : [HP3     ]
	---
[ INFO] [1660394742.377817900]: [test_armor_tools] esiste HP3? (atteso: yes) -> yes
[ INFO] [1660394742.377910200]:    Print Response: 
	success : true
	timeout : false
	exit_code : 0
	error_description : 
	is_consistent : true
	queried_objects :
	-	<http://www.emarolab.it/cluedo-ontology#INCONSISTENT>
	-	<http://www.emarolab.it/cluedo-ontology#COMPLETED>
	-	<http://www.emarolab.it/cluedo-ontology#HYPOTHESIS>
	sparql_queried_objects : 
	---
[ INFO] [1660394742.377993500]: [test_armor_tools] 


 TEST COMPLETO INTERFACCIA 



[ INFO] [1660394742.378055900]: [test_armor_tools] 

	======	TEST GetClassOfIndiv()
[ INFO] [1660394742.378113400]: [test_armor_tools] classe dell'individual Stadium(con la maiuscola, scritto correttamente, no deep search, atteso un solo elemento)
[ INFO] [1660394742.382232000]: [test_armor_tools] 	numero 1 -> PLACE
[ INFO] [1660394742.382303900]: [test_armor_tools] 	numero 2 -> HYPOTHESIS
[ INFO] [1660394742.382366900]: 
[ INFO] [1660394742.382428000]: [test_armor_tools] classe dell'individual Stadium(con la maiuscola, scritto correttamente, usando la deep search, atteso un solo elemento)
[ INFO] [1660394742.387964600]: [test_armor_tools] 	numero 1 -> PLACE
[ INFO] [1660394742.388033600]: 
[ INFO] [1660394742.388091300]: [test_armor_tools] classe dell'individual stadium(senza la maiuscola, sbagliato, no deep search, atteso 0)
[ INFO] [1660394742.393586100]: [test_armor_tools] size==0
[ INFO] [1660394742.393664900]: 
[ INFO] [1660394742.393723200]: [test_armor_tools] 

	======	TEST ExistsIndiv()
[ INFO] [1660394742.397511500]: [test_armor_tools] esiste 'stadium' senza la maiuscola? -> no
[ INFO] [1660394742.401432500]: [test_armor_tools] esiste 'Stadium' con la maiuscola? -> yes
[ INFO] [1660394742.406154300]: [test_armor_tools] esiste 'HP0' ? -> no
[ INFO] [1660394742.406253700]: [test_armor_tools] 

	======	TEST GetIndivOfClass()
[ INFO] [1660394742.406320500]: [test_armor_tools] tutti gli individuals della classe HYPOTHESIS(attesa ... un bel po' di roba)
[ INFO] [1660394742.411050000]: [test_armor_tools] 	numero 1 -> Jim
[ INFO] [1660394742.411111100]: [test_armor_tools] 	numero 2 -> 
[ INFO] [1660394742.411161000]: [test_armor_tools] 	numero 3 -> Study
[ INFO] [1660394742.411212300]: [test_armor_tools] 	numero 4 -> Gun
[ INFO] [1660394742.411269900]: [test_armor_tools] 	numero 5 -> Stadium
[ INFO] [1660394742.411323100]: [test_armor_tools] 	numero 6 -> Lounge
[ INFO] [1660394742.411372600]: [test_armor_tools] 	numero 7 -> HP3
[ INFO] [1660394742.411427000]: 
[ INFO] [1660394742.411487200]: [test_armor_tools] tutti gli individuals della classe PLACE(attesi 3 elementi)
[ INFO] [1660394742.414906100]: [test_armor_tools] 	numero 1 -> Study
[ INFO] [1660394742.414971500]: [test_armor_tools] 	numero 2 -> Stadium
[ INFO] [1660394742.415020800]: [test_armor_tools] 	numero 3 -> Lounge
[ INFO] [1660394742.415078600]: 
[ INFO] [1660394742.415134300]: [test_armor_tools] 

	======	TEST GetValuedOfIndiv()
[ INFO] [1660394742.415210700]: [test_armor_tools] propriet?? where dell'ipotesi HP3(attesi due elementi)
[ INFO] [1660394742.418685800]: [test_armor_tools] 	numero 1 -> Study
[ INFO] [1660394742.418750900]: [test_armor_tools] 	numero 2 -> Stadium
[ INFO] [1660394742.418811100]: 
[ INFO] [1660394742.418866600]: [test_armor_tools] 

	======	TEST RemoveHypothesis()
[ WARN] [1660394742.418928600]: [test_armor_tools] NOTA BENE: il comando REMOVE di aRMOR ?? buggato, non funziona. Per farlo funzionare occorre trovare un workaround nell'interfaccia
[ INFO] [1660394742.418987900]: [test_armor_tools] aggiungo l'ipotesi HPtoremove (uso qui solo comandi dell'interfaccia, non i diretti di aRMOR)
[ INFO] [1660394742.448092200]: [test_armor_tools] esiste 'HPtoremove' ? -> yes
[ INFO] [1660394742.448175000]: [test_armor_tools] classe dell'individual HPtoremove? (atteso un solo elemento)
[ INFO] [1660394742.451924800]: [test_armor_tools] 	numero 1 -> HYPOTHESIS
[ INFO] [1660394742.451996600]: [test_armor_tools] rimozione
[ INFO] [1660394742.457330500]: [test_armor_tools] RemoveHypothesis( ) = yes
[ INFO] [1660394742.465532300]: [test_armor_tools] esiste ancora 'HPtoremove' ? -> no
[ INFO] [1660394742.465603300]: [test_armor_tools] 

	======	TEST FindCompleteHypotheses()
[ INFO] [1660394742.470117800]: [test_armor_tools] ricerca delle ipotesi complete (atteso: una sola ipotesi, escludendo quella scartata
[ INFO] [1660394742.470200300]: [test_armor_tools] 	numero 1 -> HP3
[ INFO] [1660394742.470265300]: [test_armor_tools] 

	======	TEST FindInconsistentHypotheses()
[ INFO] [1660394742.474777500]: [test_armor_tools] ricerca delle ipotesi inconsistenti (atteso: solo una, o anche due, includendo quella scartata)
[ INFO] [1660394742.474843000]: [test_armor_tools] 	numero 1 -> HP3
[ INFO] [1660394742.597462300]: [test_armor_tools] THAT'S ALL FOLKS!

```

### ROSPlan -- PDDL model

**popf output (first goal):**

```text
root@3b17871017fd:~/ros_ws/src/erl2-new/robocluedo_rosplan/pddl/robocluedo# ./solve_popf.sh 
Number of literals: 28
Constructing lookup tables: [10%] [20%] [30%] [40%] [50%] [60%] [70%] [80%] [90%] [100%] [110%] [120%] [130%] [140%] [150%] [160%] [170%]
Post filtering unreachable actions:  [10%] [20%] [30%] [40%] [50%] [60%] [70%] [80%] [90%] [100%] [110%] [120%] [130%] [140%] [150%] [160%] [170%]
No analytic limits found, not considering limit effects of goal-only operators
64% of the ground temporal actions in this problem are compression-safe
Initial heuristic = 2.000
b (1.000 | 0.990);;;; Solution Found
; States evaluated: 3
; Cost: 0.990
; Time 0.00
0.000: (replan)  [0.990]
```

**popf output (second goal):**

```text
root@3b17871017fd:~/ros_ws/src/erl2-new/robocluedo_rosplan/pddl/robocluedo# ./solve_popf.sh 
Number of literals: 28
Constructing lookup tables: [10%] [20%] [30%] [40%] [50%] [60%] [70%] [80%] [90%] [100%] [110%] [120%] [130%] [140%] [150%] [160%] [170%]
Post filtering unreachable actions:  [10%] [20%] [30%] [40%] [50%] [60%] [70%] [80%] [90%] [100%] [110%] [120%] [130%] [140%] [150%] [160%] [170%]
No analytic limits found, not considering limit effects of goal-only operators
64% of the ground temporal actions in this problem are compression-safe
Initial heuristic = 11.000
b (10.000 | 0.990)b (9.000 | 1.981)b (8.000 | 2.972)b (7.000 | 5.945)b (6.000 | 5.945)b (5.000 | 6.936)b (4.000 | 7.927)b (3.000 | 8.918)b (2.000 | 8.918)b (1.000 | 9.909);;;; Solution Found
; States evaluated: 21
; Cost: 10.900
; Time 0.00
0.000: (replan)  [0.990]
0.991: (move-to center wp1)  [0.990]
1.982: (signal-collect-hint wp1)  [0.990]
2.973: (collect-hint wp1)  [0.990]
3.964: (move-to wp1 wp2)  [0.990]
4.955: (move-to wp2 wp3)  [0.990]
5.946: (signal-collect-hint wp3)  [0.990]
6.937: (collect-hint wp3)  [0.990]
7.928: (move-to wp3 wp4)  [0.990]
8.919: (move-to-center wp4 center)  [0.990]
9.910: (solve)  [0.990]

```

### simple ROSPlan pipeline

**SHELL 1:**

```text
[ INFO] [1660396623.534248300]: KCL: (/rosplan_knowledge_base) Starting a PDDL Knowledge Base
process[popf_planner_interface-5]: started with pid [29974]
process[rosplan_parsing_interface-6]: started with pid [29983]
process[rosplan_plan_dispatcher-7]: started with pid [29987]
[ INFO] [1660396623.553958400]: KCL: (/rosplan_problem_interface) Ready to receive
process[rosplan_interface_replan-8]: started with pid [29995]
process[rosplan_interface_move_to-9]: started with pid [30000]
process[rosplan_interface_signal_collect_hint-10]: started with pid [30005]
process[rosplan_interface_collect_hint-11]: started with pid [30011]
process[rosplan_interface_move_to_center-12]: started with pid [30016]
process[rosplan_interface_solve-13]: started with pid [30021]
[ INFO] [1660396623.605888600]: KCL: (/rosplan_parsing_interface) Ready to receive
[ INFO] [1660396623.625367500]: KCL: (/rosplan_planner_interface) Ready to receive
[ INFO] [1660396623.628658600]: KCL: (/rosplan_knowledge_base) Parsing domain
[ INFO] [1660396623.628842800]: KCL: (/rosplan_knowledge_base) Parsing domain: /root/ros_ws/src/erl2-new/robocluedo_rosplan/pddl/robocluedo/robocluedo_domain.pddl.
[ INFO] [1660396623.629704400]: KCL: (/rosplan_knowledge_base) Parsing initial state
[ INFO] [1660396623.629786800]: KCL: (/rosplan_knowledge_base) Parsing Problem File: /root/ros_ws/src/erl2-new/robocluedo_rosplan/pddl/robocluedo/robocluedo_problem.pddl.
[ INFO] [1660396623.630342900]: KCL: (/rosplan_knowledge_base) Ready to receive
[ INFO] [1660396623.650709100]: KCL: (/popf_planner_interface) Ready to receive
[ INFO] [1660396623.667384500]: KCL: (/rosplan_plan_dispatcher) Ready to receive
[ INFO] [1660396623.676432100]: KCL: (replan) Ready to receive
[ INFO] [1660396623.706270300]: KCL: (signal-collect-hint) Ready to receive
[ INFO] [1660396623.706488200]: KCL: (solve) Ready to receive
[ INFO] [1660396623.727061100]: KCL: (move-to-center) Ready to receive
[ INFO] [1660396623.727229700]: KCL: (collect-hint) Ready to receive
[ INFO] [1660396623.743870700]: KCL: (move-to) Ready to receive
[ INFO] [1660396630.438992500]: KCL: (/rosplan_problem_interface) (OUT.pddl) Generating problem file.
[ INFO] [1660396630.780932100]: KCL: (/rosplan_problem_interface) (OUT.pddl) The problem was generated.
[ INFO] [1660396630.781449900]: KCL: (/rosplan_planner_interface) Problem received.
[ INFO] [1660396632.842210000]: KCL: (/rosplan_planner_interface) (OUT.pddl) Writing problem to file.
[ INFO] [1660396632.842406600]: KCL: (/rosplan_planner_interface) (OUT.pddl) Running: timeout 10 /root/ros_ws/src/ROSPlan/rosplan_planning_system/common/bin/popf -T /root/ros_ws/src/erl2-new/robocluedo_rosplan/pddl/robocluedo/robocluedo_domain.pddl /root/ros_ws/src/erl2-new/robocluedo_rosplan/data/OUT.pddl > /root/ros_ws/src/erl2-new/robocluedo_rosplan/data/plan.pddl
[ INFO] [1660396632.846124800]: KCL: (/rosplan_planner_interface) (OUT.pddl) Planning complete
[ INFO] [1660396632.846216600]: KCL: (/rosplan_planner_interface) (OUT.pddl) Plan was solved.
[ INFO] [1660396632.846425800]: KCL: (/rosplan_parsing_interface) Planner output received.
[ INFO] [1660396634.813932000]: KCL: (/rosplan_parsing_interface) Parsing planner output.
[ INFO] [1660396634.814199200]: KCL: (/rosplan_plan_dispatcher) Plan received.
[ INFO] [1660396636.957165700]: KCL: (/rosplan_plan_dispatcher) Dispatching plan
[ INFO] [1660396636.980670300]: KCL: (/rosplan_plan_dispatcher) Dispatching action [0, replan(), 2.142903, 0.990000]
[ INFO] [1660396636.981163300]: KCL: (replan) action received
[ INFO] [1660396636.981305300]: KCL: (replan) Action completing with probability 1 and duration 1
[ INFO] [1660396637.057456100]: KCL: (/rosplan_plan_dispatcher) Feedback received [0, 1]
[ INFO] [1660396637.057576200]: KCL: (/rosplan_plan_dispatcher) Feedback received [0, 1]
[ INFO] [1660396637.981581400]: KCL: (replan) action completed successfully
[ INFO] [1660396637.990541600]: KCL: (/rosplan_knowledge_base) Removing Fact (dirty,0)
[ INFO] [1660396637.990673000]: KCL: (/rosplan_knowledge_base) Adding fact (not-dirty, 0)
[ INFO] [1660396638.057318500]: KCL: (/rosplan_plan_dispatcher) Feedback received [0, 2]
[ INFO] [1660396638.157326600]: KCL: (/rosplan_plan_dispatcher) Dispatch complete.

```

**SHELL 2:**

```text
root@3b17871017fd:~/ros_ws/src/erl2-new# rosservice call /rosplan_problem_interface/problem_generation_server

root@3b17871017fd:~/ros_ws/src/erl2-new# rostopic echo /rosplan_problem_interface/problem_instance -n 1 -p
%time,field.data
1660396631800734996,(define (problem task)
(:domain robocluedo)
(:objects
    center wp1 wp2 wp3 wp4 - waypoint
)
(:init
    (dirty)


    (is-center center)

    (not-is-center wp1)
    (not-is-center wp2)
    (not-is-center wp3)
    (not-is-center wp4)

    (at center)

    (passage center wp1)
    (passage wp1 center)
    (passage center wp4)
    (passage wp4 center)
    (passage wp1 wp2)
    (passage wp2 wp3)
    (passage wp3 wp4)

    (explored center)

    (not-explored wp1)
    (not-explored wp2)
    (not-explored wp3)
    (not-explored wp4)

    (at-center)



    (not-hint-ready)


    (not-hint-collected wp1)
    (not-hint-collected wp2)
    (not-hint-collected wp3)
    (not-hint-collected wp4)

)
(:goal (and
    (not-dirty)
))
)

root@3b17871017fd:~/ros_ws/src/erl2-new# rosservice call /rosplan_planner_interface/planning_server

root@3b17871017fd:~/ros_ws/src/erl2-new# rostopic echo /rosplan_planner_interface/planner_output -p -n 1
%time,field.data
1660396633868040800,0.000: (replan)  [0.990]

root@3b17871017fd:~/ros_ws/src/erl2-new# rosservice call /rosplan_parsing_interface/parse_plan

root@3b17871017fd:~/ros_ws/src/erl2-new# rostopic echo /rosplan_parsing_interface/complete_plan -n 1
plan: 
  - 
    action_id: 0
    plan_id: 0
    name: "replan"
    parameters: []
    duration: 0.9900000095367432
    dispatch_time: 0.0
---
root@3b17871017fd:~/ros_ws/src/erl2-new# rosservice call /rosplan_plan_dispatcher/dispatch_plan "{}" 
success: True
goal_achieved: True

```

### ???

**SHELL 1:**

```text

```

**SHELL 2:**

```text

```
