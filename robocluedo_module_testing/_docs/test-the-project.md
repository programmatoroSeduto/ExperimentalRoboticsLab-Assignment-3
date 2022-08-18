
# Testing the project -- RoboCLuedo no.3

---

```{toctree}
---
caption: Contents
maxdepth: 3
---
./test-the-project.md
```

---

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

another sequence:

```bash
rosservice call /robocluedo/pipeline_manager "{load_problem: true, solve_problem: true, parse_plan: true, execute_plan: true, landmark: 0}" 
rosservice call /robocluedo/pipeline_manager "{load_problem: true, solve_problem: true, parse_plan: true, execute_plan: true, landmark: 1}" 
rosservice call /robocluedo/pipeline_manager "{load_problem: true, solve_problem: true, parse_plan: true, execute_plan: true, landmark: 1}" 
rosservice call /robocluedo/pipeline_manager "{load_problem: true, solve_problem: true, parse_plan: true, execute_plan: true, landmark: 2}" 
rosservice call /robocluedo/pipeline_manager "{load_problem: true, solve_problem: true, parse_plan: true, execute_plan: true, landmark: 0}" 
rosservice call /robocluedo/pipeline_manager "{load_problem: true, solve_problem: true, parse_plan: true, execute_plan: true, landmark: 1}" 

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
rosservice call /robocluedo/pipeline_manager "{load_problem: true, solve_problem: true, parse_plan: true, execute_plan: true, landmark: 1}" 
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
rosservice call /robocluedo/pipeline_manager "{load_problem: true, solve_problem: true, parse_plan: true, execute_plan: true, landmark: 1}" 
rosservice call /robocluedo/pipeline_manager "{load_problem: true, solve_problem: true, parse_plan: true, execute_plan: true, landmark: 1}" 
rosservice call /robocluedo/pipeline_manager "{load_problem: true, solve_problem: true, parse_plan: true, execute_plan: true, landmark: 1}" 
rosservice call /robocluedo/pipeline_manager "{load_problem: true, solve_problem: true, parse_plan: true, execute_plan: true, landmark: 2}"

# NON RISOLVIBILE
rosservice call /robocluedo/pipeline_manager "{load_problem: true, solve_problem: true, parse_plan: true, execute_plan: true, landmark: 1}"

rosservice call /robocluedo/pipeline_manager "{load_problem: true, solve_problem: true, parse_plan: true, execute_plan: true, landmark: 1}" 
rosservice call /robocluedo/pipeline_manager "{load_problem: true, solve_problem: true, parse_plan: true, execute_plan: true, landmark: 0}" 
rosservice call /robocluedo/pipeline_manager "{load_problem: true, solve_problem: true, parse_plan: true, execute_plan: true, landmark: 1}" 
rosservice call /robocluedo/pipeline_manager "{load_problem: true, solve_problem: true, parse_plan: true, execute_plan: true, landmark: 1}" 
rosservice call /robocluedo/pipeline_manager "{load_problem: true, solve_problem: true, parse_plan: true, execute_plan: true, landmark: 2}"

```

## TESTING PKG robocluedo_robot_hunter

### BEFORE THE TESTS...

make sure to have the updated version of the model. 

- shell: `roscd robocluedo_urdf_model` and `cd robot`
- run the script `./generate_mode.sh` : the model should appear as console output

### the most simple test!

*try and guess?* let's try to ... execute the main launch file. You should not see something strange ... or, worse, errors or crashes. 

**first test:** Gazebo only

```bash
roslaunch robocluedo_robot_hunter gazebo.launch

```

**second test:** RViz only

```bash
roslaunch robocluedo_robot_hunter demo.launch rviz_config_file:=moveit_2.rviz

```

**Third test:** Gazebo and RViz (it should generate loads of warnings on the console ... )

```bash
roslaunch robocluedo_robot_hunter demo_gazebo.launch rviz_config_file:=moveit_2.rviz

```

the same launcher, without warnings (not necessairly a good thing...):

```bash
roslaunch robocluedo_robot_hunter demo_gazebo.launch rviz_config_file:=moveit_2.rviz 2>/dev/null

```

## TESTING PKG robocluedo_vision

```{note}
here are only the basic tests. to see if ArUco is detecting the markers, it is needed to run the entire architecture. see later in this document.
```

### running aruco detection

```bash
# shell 1
roslaunch robocluedo_robot_hunter run.launch world_name:=assignment3.world 2>/dev/null

# shell 2
rosrun robocluedo_vision aruco_detection

```

### aruco decoder

```bash
# shell 1
roslaunch exp_assignment3 run.launch

# shell 2
rosrun robocluedo_vision aruco_decode.py

# shell 3
rostopic echo /oracle_hint

# shell 4
rostopic pub --once /aruco_detected_ids std_msgs/Int32 "data: 0"
rostopic pub --once /aruco_detected_ids std_msgs/Int32 "data: 1"
rostopic pub --once /aruco_detected_ids std_msgs/Int32 "data: 2"
rostopic pub --once /aruco_detected_ids std_msgs/Int32 "data: 3"
rostopic pub --once /aruco_detected_ids std_msgs/Int32 "data: 4"
rostopic pub --once /aruco_detected_ids std_msgs/Int32 "data: 5"

```

## TESTING PKG robocluedo_movement_controller

the package contains nodes for performing the navigation and the manipulation. 

### go to point

this py node performs a simple navigation between two points in a path without obstacles.

```bash
# shell 1
roslaunch robocluedo_robot_hunter gazebo.launch world_name:=square_room.world 2>/dev/null

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
roslaunch robocluedo_robot_hunter gazebo.launch world_name:=square_room.world 2>/dev/null

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

```{warning}
The node `wall_follow_service_m.py` will be removed soon: it is a very poor solution for path planning. better to use move_base for moving the robot in indoor environments. 
```

```bash
# shell 1
roslaunch robocluedo_robot_hunter gazebo.launch world_name:=indoor.world 2>/dev/null

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
roslaunch robocluedo_robot_hunter gazebo.launch world_name:=square_room.world 2>/dev/null

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

### testing navigation manager -- bug_m controller

the so called node *navigation manager* allows

- to interact with the navigation system making an abstraction between the navigation command and the real component implementing the navigation
- to change at runtine navigation component in order to perform different kinds of movement depending on the situation


```bash
# shell 1
roslaunch robocluedo_robot_hunter gazebo.launch world_name:=square_room.world 2>/dev/null

# shell 2
# roslaunch robocluedo_movement_controller navigation_system.launch
roslaunch robocluedo_movement_controller navigation_system.launch launch_nav_controller:=false

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
roslaunch robocluedo_robot_hunter run.launch world_name:=square_room.world 2>/dev/null

# shell 2
rosrun robocluedo_movement_controller manipulation_controller 2>/dev/null

# shell 3
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
rosservice call /tip_pos "command: 5" 
sleep 2
rosservice call /tip_pos "command: 6" 
sleep 2
rosservice call /tip_pos "command: 7" 
sleep 2
rosservice call /tip_pos "command: 8" 
sleep 2

# UNKNOWN POSE
rosservice call /tip_pos "command: 9" 
sleep 2

# UNKNOWN POSE
rosservice call /tip_pos "command: -1" 
sleep 2

```

### manipulation controller -- async mode

thye manipulation controller can work also in the so called "async mode": simply the node asking for a certain pose, publishes a message to the manipulation controller, which applies the command. the node sending the request shall not wait for the new posture. 

```bash
# shell 1
roslaunch robocluedo_robot_hunter run.launch world_name:=square_room.world 2>/dev/null

# shell 2
rosrun robocluedo_movement_controller manipulation_controller 2>/dev/null

# shell 3
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
rosservice call /tip_pos "command: 5" 
sleep 2
rosservice call /tip_pos "command: 6" 
sleep 2
rosservice call /tip_pos "command: 7" 
sleep 2
rosservice call /tip_pos "command: 8" 
sleep 2

# UNKNOWN POSE
rosservice call /tip_pos "command: 9" 
sleep 2

# UNKNOWN POSE
rosservice call /tip_pos "command: -1" 
sleep 2

```

another version of the same test:

```bash
# shell 1
roslaunch robocluedo_robot_hunter run.launch world_name:=indoor.world 2>/dev/null

# shell 2
rosrun robocluedo_movement_controller manipulation_controller 2>/dev/null

# shell 3
rosrun robocluedo_mission_manager manipulation_unit

# shell 4
rosservice list | grep tip
# /tip_pos
# /tip_pos_auto_switch

rosservice call /tip_pos_auto_switch "data: true"
# 6 different poses
sleep 60 
rosservice call /tip_pos_auto_switch "data: false"

```

## TESTING rosplan plus real motion controller

it is the same last test of robocluedo ROSPlan, but this time using the real movement controller. 

```bash
# shell 1
roslaunch robocluedo_mission_manager run_components.launch launch_robot:=true launch_rosplan:=true launch_motion_controllers:=true launch_oracle:=true launch_mission_manager_components:=true launch_mission_manager:=false launch_aruco:=false 2>/dev/null

# shell 2
rosservice call /robocluedo/pipeline_manager "{load_problem: true, solve_problem: true, parse_plan: true, execute_plan: true, landmark: 0}" 
rosservice call /robocluedo/pipeline_manager "{load_problem: true, solve_problem: true, parse_plan: true, execute_plan: true, landmark: 1}" 
rosservice call /robocluedo/pipeline_manager "{load_problem: true, solve_problem: true, parse_plan: true, execute_plan: true, landmark: 1}" 
rosservice call /robocluedo/pipeline_manager "{load_problem: true, solve_problem: true, parse_plan: true, execute_plan: true, landmark: 1}" 
rosservice call /robocluedo/pipeline_manager "{load_problem: true, solve_problem: true, parse_plan: true, execute_plan: true, landmark: 1}" 
rosservice call /robocluedo/pipeline_manager "{load_problem: true, solve_problem: true, parse_plan: true, execute_plan: true, landmark: 1}" 
rosservice call /robocluedo/pipeline_manager "{load_problem: true, solve_problem: true, parse_plan: true, execute_plan: true, landmark: 1}" 
rosservice call /robocluedo/pipeline_manager "{load_problem: true, solve_problem: true, parse_plan: true, execute_plan: true, landmark: 2}"

rosservice call /robocluedo/pipeline_manager "{load_problem: true, solve_problem: true, parse_plan: true, execute_plan: true, landmark: 0}" 
rosservice call /robocluedo/pipeline_manager "{load_problem: true, solve_problem: true, parse_plan: true, execute_plan: true, landmark: 1}" 
rosservice call /robocluedo/pipeline_manager "{load_problem: true, solve_problem: true, parse_plan: true, execute_plan: true, landmark: 1}" 
rosservice call /robocluedo/pipeline_manager "{load_problem: true, solve_problem: true, parse_plan: true, execute_plan: true, landmark: 1}" 
rosservice call /robocluedo/pipeline_manager "{load_problem: true, solve_problem: true, parse_plan: true, execute_plan: true, landmark: 1}" 
rosservice call /robocluedo/pipeline_manager "{load_problem: true, solve_problem: true, parse_plan: true, execute_plan: true, landmark: 1}" 
rosservice call /robocluedo/pipeline_manager "{load_problem: true, solve_problem: true, parse_plan: true, execute_plan: true, landmark: 1}" 
rosservice call /robocluedo/pipeline_manager "{load_problem: true, solve_problem: true, parse_plan: true, execute_plan: true, landmark: 2}"

```

## TESTING AI, motion and ArUco

### RCL2 components and ArUco (no armor)

```bash
# shell 1
roslaunch robocluedo_mission_manager run_components.launch launch_robot:=true launch_rosplan:=true launch_motion_controllers:=true launch_oracle:=true launch_aruco:=true launch_mission_manager_components:=true launch_mission_manager:=false 2>/dev/null

# shell 2
rosservice call /robocluedo/pipeline_manager "{load_problem: true, solve_problem: true, parse_plan: true, execute_plan: true, landmark: 0}" 
rosservice call /robocluedo/pipeline_manager "{load_problem: true, solve_problem: true, parse_plan: true, execute_plan: true, landmark: 1}" 
rosservice call /robocluedo/pipeline_manager "{load_problem: true, solve_problem: true, parse_plan: true, execute_plan: true, landmark: 2}" 

```

### ARUCO unit separated

```bash
# shell 1
roslaunch robocluedo_mission_manager run_components.launch launch_robot:=true launch_rosplan:=true launch_motion_controllers:=true launch_oracle:=true launch_aruco:=false launch_mission_manager_components:=true launch_mission_manager:=false 2>/dev/null

# shell 2
roslaunch robocluedo_vision run.launch

# shell 2
rostopic echo /oracle_hint

# shell 4
rosservice call /robocluedo/pipeline_manager "{load_problem: true, solve_problem: true, parse_plan: true, execute_plan: true, landmark: 0}" 
rosservice call /robocluedo/pipeline_manager "{load_problem: true, solve_problem: true, parse_plan: true, execute_plan: true, landmark: 1}" 
rosservice call /robocluedo/pipeline_manager "{load_problem: true, solve_problem: true, parse_plan: true, execute_plan: true, landmark: 1}" 
rosservice call /robocluedo/pipeline_manager "{load_problem: true, solve_problem: true, parse_plan: true, execute_plan: true, landmark: 1}" 
rosservice call /robocluedo/pipeline_manager "{load_problem: true, solve_problem: true, parse_plan: true, execute_plan: true, landmark: 1}" 
rosservice call /robocluedo/pipeline_manager "{load_problem: true, solve_problem: true, parse_plan: true, execute_plan: true, landmark: 1}" 
rosservice call /robocluedo/pipeline_manager "{load_problem: true, solve_problem: true, parse_plan: true, execute_plan: true, landmark: 2}" 

```

### hints detection

a variant of the abovementioned test which allows to see the hints from the unit `aruco_decode.py`

```bash
# shell 1
roslaunch robocluedo_mission_manager run_components.launch launch_robot:=true launch_rosplan:=true launch_motion_controllers:=true launch_oracle:=true launch_aruco:=true launch_mission_manager_components:=true launch_mission_manager:=false 2>/dev/null

# shell 2
rostopic echo /oracle_hint

# shell 3
rosservice call /robocluedo/pipeline_manager "{load_problem: true, solve_problem: true, parse_plan: true, execute_plan: true, landmark: 0}" 
rosservice call /robocluedo/pipeline_manager "{load_problem: true, solve_problem: true, parse_plan: true, execute_plan: true, landmark: 1}" 
rosservice call /robocluedo/pipeline_manager "{load_problem: true, solve_problem: true, parse_plan: true, execute_plan: true, landmark: 1}" 
rosservice call /robocluedo/pipeline_manager "{load_problem: true, solve_problem: true, parse_plan: true, execute_plan: true, landmark: 1}" 
rosservice call /robocluedo/pipeline_manager "{load_problem: true, solve_problem: true, parse_plan: true, execute_plan: true, landmark: 1}" 
rosservice call /robocluedo/pipeline_manager "{load_problem: true, solve_problem: true, parse_plan: true, execute_plan: true, landmark: 1}" 
rosservice call /robocluedo/pipeline_manager "{load_problem: true, solve_problem: true, parse_plan: true, execute_plan: true, landmark: 2}" 

```

### hints detection and movements of the arm

let's add the authomatic movements of the arm during the exploration of the environment:

```bash
# shell 1
roslaunch robocluedo_mission_manager run_components.launch launch_robot:=true launch_rosplan:=true launch_motion_controllers:=true launch_oracle:=true launch_aruco:=true launch_mission_manager_components:=true launch_mission_manager:=false 2>/dev/null

# shell 2
rostopic echo /oracle_hint

# shell 3
rosservice call /tip_pos_auto_switch "data: true"
rosservice call /robocluedo/pipeline_manager "{load_problem: true, solve_problem: true, parse_plan: true, execute_plan: true, landmark: 0}" 
rosservice call /robocluedo/pipeline_manager "{load_problem: true, solve_problem: true, parse_plan: true, execute_plan: true, landmark: 1}" 
rosservice call /robocluedo/pipeline_manager "{load_problem: true, solve_problem: true, parse_plan: true, execute_plan: true, landmark: 1}" 
rosservice call /robocluedo/pipeline_manager "{load_problem: true, solve_problem: true, parse_plan: true, execute_plan: true, landmark: 1}" 
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
