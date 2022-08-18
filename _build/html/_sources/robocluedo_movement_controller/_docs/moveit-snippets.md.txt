# moveit snippets

*directly from the official tutorial...*

---

```{toctree}
---
maxdepth: 3
caption: Contents
---
./moveit-snippets.md
```

---

## headers and Setup

### Basic

The most simple set of headers: 

```c++
#include <ros/ros.h>

#include <moveit/move_group_interface/move_group_interface.h>
#include <moveit/planning_scene_interface/planning_scene_interface.h>
```

a more comprehensive set of headers, see also RViz visualization tools:

```c++
#include <ros/ros.h>

#include <moveit/move_group_interface/move_group_interface.h>
#include <moveit/planning_scene_interface/planning_scene_interface.h>
#include <moveit_visual_tools/moveit_visual_tools.h>
#include <moveit_msgs/DisplayRobotState.h>
#include <moveit_msgs/DisplayTrajectory.h>
#include <moveit_msgs/AttachedCollisionObject.h>
#include <moveit_msgs/CollisionObject.h>
```

### compile with catkin

just add these dependencies:

```cmake
find_package(catkin REQUIRED COMPONENTS
  roscpp
  ...
  moveit_msgs
  moveit_core
  moveit_ros_perception
  moveit_ros_planning
  moveit_ros_planning_interface
  )
```

### Init the node with AsyncSpin

It is mandatory in order to call the methods for the robo movement. 

```c++
// run the node as AsyncSpinner
// ros::init( ... )
// ros::AsyncSpinner spinner( n_threads );

ros::AsyncSpinner spinner(1);
spinner.start( );

// spin becomes...
ros::waitForShutdown( );
```

**Remember to use AsyncSpin!** 

## A MoveIt OOP pattern for ROS1

?? remember to replace `???` with a name suitable for your implementation!

Feel free to structure the workflow of the node using this first model, ready to be tailored to the desired functionalities. 

```c++

#include "ros/ros.h"
#include <signal.h>
#include <string>

#define NODE_NAME ???

#include <moveit/move_group_interface/move_group_interface.h>
#include <moveit/planning_scene_interface/planning_scene_interface.h>
#include <moveit_visual_tools/moveit_visual_tools.h>
#include <moveit_msgs/DisplayRobotState.h>
#include <moveit_msgs/DisplayTrajectory.h>
#include <moveit_msgs/AttachedCollisionObject.h>
#include <moveit_msgs/CollisionObject.h>

#define LOGSQUARE( str )  "[" << str << "] "
#define OUTLABEL          LOGSQUARE( NODE_NAME )
#define TLOG( msg )       ROS_INFO_STREAM( OUTLABEL << msg )
#define TWARN( msg )      ROS_WARN_STREAM( OUTLABEL << "WARNING: " << msg )
#define TERR( msg )       ROS_WARN_STREAM( OUTLABEL << "ERROR: " << msg )


class ???
{
public:
	
	???( std::string group_name ) : 
		planning_group( group_name )
	{
		// init MoveIt groups
		static const std::string PLANNING_GROUP = "panda_arm";
		mgi = move_group_interface(PLANNING_GROUP);
		
		// MoveIt settings
		mgi.setPlanningTime(10.0);
		// ...
	}
	
	void spin( int n_loops_per_sec = 1 )
	{
		// TODO main functionality (even a simple spin)
		
		/*
		ros::spin( )
		*/
		
		/*
		ros::Rate freq( n_loops_per_sec );
		while( ros::ok( ) )
		{
			// ... do whatever you want
			
			// spin and wait
			ros::spin_once( );
			freq.sleep( );
		}
		*/
	}
	
	// other ModeIt specific functionalities
	// ...
	
	// ...
	
private:
	
	// ROS node handle
    ros::NodeHandle nh;
	
	// moveIt group name
	std::string planning_group;
	
	// planning interface
	moveit::planning_interface::MoveGroupInterface mgi;
	
	// previously generated plan
	moveit::planning_interface::MoveGroupInterface::Plan plan;
	
	// ...
}


void shut_msg( )
{
	TLOG( "stopping... " );
	ros::shutdown( );
}


int main( int argc, char* argv[] )
{
	ros::init( argc, argv, NODE_NAME, ros::init_options::NoSigintHandler );
	signal( SIGINT, shut_msg );
	ros::NodeHandle nh;
	
	// required by moveIt: run the node as AsyncSpinner
	ros::AsyncSpinner spinner(1);
	spinner.start( );
	
	TLOG( "starting ... " );
	
	// TODO define here services and topics ...
	
	TLOG( "ready" );
	
	// TODO the functionality of the node ... 
	( ???( ) ).spin( );
	
	return 0;
}
```

## MoveIt -- ROS1 C++ snippets

Feel free to go deeper into the details of code, and to combine the snippets. Each snippet incude a particular feature, which can be joined with the other ones in order to create the entire application. 

### Move Group Interface Settings

?? You can find many other options [here](http://docs.ros.org/en/noetic/api/moveit_ros_planning_interface/html/classmoveit_1_1planning__interface_1_1MoveGroupInterface.html).

Here are only the most common properties you maybe want to explicitly set for the move group interface. Inside the above mentioned page you can find also the getters, as well as many other useful options. 

```c++
static const std::string PLANNING_GROUP = "your_planning_group";
moveit::planning_interface::MoveGroupInterface group(PLANNING_GROUP);

// also the "get" are available for these methods
group.setEndEffectorLink("end_effector_link");
group.setPoseReferenceFrame("base_link");
group.setPlannerId("RRTstar");
group.setNumPlanningAttempts(10);
group.setPlanningTime(10.0);
group.allowReplanning(true);
group.setGoalJointTolerance(0.0001);
group.setGoalPositionTolerance(0.0001);
group.setGoalOrientationTolerance(0.001);
```

### Just move that!

Notice that `.move()` and `.execute(my_plan)` are blocking calls, and the snippets here don't allow to fix a sequence of movements, hence you should manually create a loop for reaching a sequnce of poses. 

Also notice that the MoveIt code is often executed by a specific module, hence a blocking call is accettable in many cases, without loosing parallelism between planning and movement. In particular, if you want to have a non-blocking interface, a good idea could be to create a service in another node, and every time ask the state of the robot to it. 

In case you really need non-blocking calls, consider to use [`asyncExecute`](http://docs.ros.org/en/lunar/api/moveit_ros_planning_interface/html/classmoveit_1_1planning__interface_1_1MoveGroupInterface.html#a07227c4302a2a4d423ba413bde4deab7). 

```c++
// STEP 1 -- reference to the group to move
//    (the model is loaded from the parameter server)
static const std::string PLANNING_GROUP = "panda_arm";
moveit::planning_interface::MoveGroupInterface move_group_interface(PLANNING_GROUP);
// STEP 2 -- declare the final pose of the end effector
geometry_msgs::Pose target_pose1;
target_pose1.orientation.w = 1.0;
target_pose1.position.x = 0.28;
target_pose1.position.y = -0.2;
target_pose1.position.z = 0.5;
move_group_interface.setPoseTarget(target_pose1);

// AND FINALLY MOVE THAT!
move_group_interface.move(); // blocking
```

### Move to a preset pose

very simple: just use `group.setNamedTarget("your_pose_name");` to set the target. 

### Single Waypoint Path -- MoveIt with explicit planning

This snippet doesn't specify in which space the planner has to work. If you want specifically cartesian planning or joint space planning, go to the other snippets. 

```cpp
// STEP 1 -- ...
// STEP 2 -- ...
// STEP 3 -- planning
moveit::planning_interface::MoveGroupInterface::Plan my_plan;
//    you can set a timeout for the planning using .setPlanningTime
//    default is 5
move_group_interface.setPlanningTime(10.0);
// plan
bool success = (
    move_group_interface.plan(my_plan) == moveit::planning_interface::MoveItErrorCode::SUCCESS);

// AND FINALLY MOVE THAT!
move_group_interface.execute(my_plan); // blocking
```

### Single Waypoint Path -- Planning in the Joint Space (first way)

This piece of code allows to move the group in the neighborhood of the current configuration:

```cpp
// STEP 1 -- ...
// STEP 2a -- current cofiguration and next configuration
moveit::core::RobotStatePtr current_state = move_group_interface.getCurrentState();
std::vector<double> joint_group_positions;
current_state->copyJointGroupPositions(joint_model_group, joint_group_positions);
// STEP 2b -- set the next configuration
//    joint_group_positions[0] = ...
move_group_interface.setJointValueTarget(joint_group_positions);
// STEP 3 -- set movement constraints
move_group_interface.setMaxVelocityScalingFactor(0.05);
move_group_interface.setMaxAccelerationScalingFactor(0.05);

// AND FINALLY MOVE THAT!
//    (here you can use move() as well)
moveit::planning_interface::MoveGroupInterface::Plan my_plan;
bool success = (
   move_group_interface.plan(my_plan) == moveit::planning_interface::MoveItErrorCode::SUCCESS);
move_group_interface.execute(my_plan); // blocking
```

### planning in the joint space (second method)

This method explicitly involves the Inverse Kinematin in order to plan the path. It's preferrable when you want to override any unknown MoveIt settings in the system you're using for your project, i.e. when you don't know which default planning is set. 

```cpp
// STEP 1 -- set the starting pose of the tip
geometry_msgs::Pose start_pose;
start_pose.orientation.w = 1.0;
start_pose.position.x = 0.55;
start_pose.position.y = -0.05;
start_pose.position.z = 0.8;

// STEP 2 -- set the target pose
geometry_msgs::Pose target_pose;
//    ... setup the target pose ...

// STEP 3a -- setup the IK
const moveit::core::JointModelGroup* joint_model_group =
      move_group_interface.getCurrentState()->getJointModelGroup(PLANNING_GROUP);
moveit::core::RobotStatePtr current_joint_state = 
      *move_group_interface.getCurrentState();
moveit::core::RobotState start_state( current_joint_state );

// STEP 3b -- start pose and target pose
start_state.setFromIK( joint_model_group, start_pose );
move_group_interface.setStartState( start_state ); // the IK is passed to the interface here
move_group_interface.setPoseTarget( target_pose );

// STEP 4 -- explicit planning
//    (but I think that even the implicit planning + movement
//    should work well here)
move_group_interface.setPlanningTime(10.0); // suggested, not mandatory
success = 
    (move_group_interface.plan(my_plan) == moveit::planning_interface::MoveItErrorCode::SUCCESS);
ROS_INFO("plan %s", success ? "" : "FAILED");

// AND FINALLY MOVE THAT!
move_group_interface.execute(my_plan);
```

### Multiple Waypoints Path -- Motion using Cartesian Paths

The cartesian path allows to move the robot across many waypoints. Each waypoint is a *pose* message; to express a path, build a `std::vector<geometry_msgs::Pose>` and `.push_back()` each intermediate waypoint. 

```cpp
// STEP 1 -- waypoints
std::vector<geometry_msgs::Pose> waypoints;

// good practise: always specify the starting pose
geometry_msgs::Pose w0;
// ... .x .y .z
waypoints.push_back( w0 );

// then specify each other waypoint
geometry_msgs::Pose w1;
// ... .x .y .z
waypoints.push_back( w1 );

// STEP 2 -- plan the trajectory
moveit_msgs::RobotTrajectory trajectory;
const double jump_threshold = 0.0;
const double eef_step = 0.01;
double fraction = move_group_interface.computeCartesianPath(
    waypoints, eef_step, jump_threshold, trajectory); // trajectory out by reference

// AND FINALLY MOVE THAT!
move_group_interface.execute(trajectory);
```

### Movement Constraints

Adding constraints to the robot movement is quite straightforward: just fill in the message [`moveit_msgs/Constraints`](http://docs.ros.org/en/api/moveit_msgs/html/msg/Constraints.html) with the constraints you want for the motion, then set the constraints in the proper set with `.push_back( constraint_instance )` and confirm with `move_group_interface.setPathConstraints( constraints_msg )`.

Here's a simple example: 

```cpp
// STEP 1 -- set the constraints as single messages
moveit_msgs::OrientationConstraint ocm;
ocm.link_name = "panda_link7";
ocm.header.frame_id = "panda_link0";
ocm.orientation.w = 1.0;
ocm.absolute_x_axis_tolerance = 0.1;
ocm.absolute_y_axis_tolerance = 0.1;
ocm.absolute_z_axis_tolerance = 0.1;
ocm.weight = 1.0;

// STEP 2 -- create the set of constraints
moveit_msgs::Constraints test_constraints;
test_constraints.orientation_constraints.push_back(ocm);

// STEP 3 -- make them effective
move_group_interface.setPathConstraints(test_constraints);
```

Here are the links to the messages:

- The main message: [moveit_msgs/Constraints.msg](moveit_msgs/Constraints.msg)
- [moveit_msgs/JointConstraint.msg](http://docs.ros.org/en/api/moveit_msgs/html/msg/JointConstraint.html)
- [moveit_msgs/PositionConstraint.msg](http://docs.ros.org/en/api/moveit_msgs/html/msg/PositionConstraint.html)
- [moveit_msgs/OrientationConstraint.msg](http://docs.ros.org/en/api/moveit_msgs/html/msg/OrientationConstraint.html)
- [moveit_msgs/VisibilityConstraint.msg](http://docs.ros.org/en/api/moveit_msgs/html/msg/VisibilityConstraint.html)

Every time you need a particular constraint, the method is always the same: just scroll the list above, and select the constraints you want. 

### [Easy](https://www.youtube.com/watch?v=LmToQ1-t98o) queries

```cpp
// current planning frame
ROS_INFO("Planning frame: %s", move_group_interface.getPlanningFrame().c_str());

// name of the end effector
ROS_INFO("End effector link: %s", move_group_interface.getEndEffectorLink().c_str());

// list of all defined groups
ROS_INFO("Available Planning Groups:");
std::copy(move_group_interface.getJointModelGroupNames().begin(),
  move_group_interface.getJointModelGroupNames().end(), 
  std::ostream_iterator<std::string>(std::cout, ", "));
```

## Useful Links

- moveit planning interface reference class [here](http://docs.ros.org/en/jade/api/moveit_ros_planning_interface/html/classmoveit_1_1planning__interface_1_1MoveGroup.html#aeffa9b03640c66c32fdf4fdda8b0dd5b)

- C++ reference for the class [`ros::AsyncSpinner`](http://docs.ros.org/en/noetic/api/roscpp/html/classros_1_1AsyncSpinner.html)
- Official (awful) documentation about [MoveIt for ROS1](https://ros-planning.github.io/moveit_tutorials/)
- An example of MoveIt cpp implementation: [source code](https://github.com/ros-planning/moveit_tutorials/blob/master/doc/move_group_interface/src/move_group_interface_tutorial.cpp)
    - [here](https://github.com/ros-planning/moveit_tutorials) the main page of moveit_tutorials on GitHub
- Planning [error codes](http://docs.ros.org/en/jade/api/moveit_ros_planning_interface/html/classmoveit_1_1planning__interface_1_1MoveItErrorCode.html)
- [`moveit::planning_interface::MoveGroupInterface::setJointValueTarget`](http://docs.ros.org/en/lunar/api/moveit_ros_planning_interface/html/classmoveit_1_1planning__interface_1_1MoveGroupInterface.html#a71e3819bd31711dcb81c18305ca9a521)
-[`moveit::planning_interface::MoveGroupInterface`](http://docs.ros.org/en/lunar/api/moveit_ros_planning_interface/html/classmoveit_1_1planning__interface_1_1MoveGroupInterface.html)
- for non-blocking `move()`, see [`asyncExecute`](http://docs.ros.org/en/lunar/api/moveit_ros_planning_interface/html/classmoveit_1_1planning__interface_1_1MoveGroupInterface.html#a07227c4302a2a4d423ba413bde4deab7)
- list of messages in the [moveit_msgs](http://wiki.ros.org/moveit_msgs) package, in particular [moveit_msgs/Constraints msg](http://docs.ros.org/en/api/moveit_msgs/html/msg/Constraints.html)
