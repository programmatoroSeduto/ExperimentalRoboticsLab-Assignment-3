# HOW TO -- navigation stack CRASH COURSE

---

```{toctree}
---
caption: concents
---
./how-to-nav-stack.md
```

---

here are some quick informations about how to set up and use the **ROS Navigation Stack**.

## install the Navigation Stack

### Slam - GMapping

In order to run the project, you need the two packages you can find [here](https://github.com/CarmineD8/slam_gmapping.git). SLAM and GMapping are tools for managing the movement of a robot with noisy odometry: their purpose is to correct odometry in a way that the robot can get its position as precisely as possible. 

Copy the packages into the workspace you prefer. **use branch : _noetic_**


```bash
git clone https://github.com/CarmineD8/slam_gmapping.git -b noetic
```

Also these packages are required. Please install them. 

```bash
sudo apt-get install ros-noetic-openslam-gmapping
sudo apt-get install ros-noetic-navigation
```

### MoveBase

MoveBase is a motion planner: given a goal, it can retrieve a path from the actual position to the desired one, recomputing the path depending on the informations gathered by sensors in conjunction with Slam-GMapping. 

Simply install it:

```bash
sudo apt-get install ros-noetic-move-base
```

## A quick example with Gazebo and RViz

see [this project on GitHub](https://github.com/programmatoroSeduto/RT1_assignment_2)

```bash
git clone https://github.com/programmatoroSeduto/RT1_assignment_2.git -b main ./test_nav_stack
```

in particular see [how to setup the project](https://github.com/programmatoroSeduto/RT1_assignment_2#how-to-set-up-the-project)

and see [how to build the project](https://github.com/programmatoroSeduto/RT1_assignment_2#build-the-project)

## Names and Types of the Interfaces of MoveBase

**services**:

```
/move_base/NavfnROS/make_plan
/move_base/TrajectoryPlannerROS/set_parameters
/move_base/clear_costmaps
/move_base/global_costmap/inflation_layer/set_parameters
/move_base/global_costmap/obstacle_layer/set_parameters
/move_base/global_costmap/set_parameters
/move_base/global_costmap/static_layer/set_parameters
/move_base/local_costmap/inflation_layer/set_parameters
/move_base/local_costmap/obstacle_layer/set_parameters
/move_base/local_costmap/set_parameters
/move_base/make_plan
/move_base/set_parameters
```

**topics:**

```
/move_base/NavfnROS/plan
/move_base/TrajectoryPlannerROS/cost_cloud
/move_base/TrajectoryPlannerROS/global_plan
/move_base/TrajectoryPlannerROS/local_plan
/move_base/TrajectoryPlannerROS/parameter_descriptions
/move_base/TrajectoryPlannerROS/parameter_updates
/move_base/cancel
/move_base/current_goal
/move_base/feedback
/move_base/global_costmap/costmap
/move_base/global_costmap/costmap_updates
/move_base/global_costmap/footprint
/move_base/global_costmap/inflation_layer/parameter_descriptions
/move_base/global_costmap/inflation_layer/parameter_updates
/move_base/global_costmap/obstacle_layer/clearing_endpoints
/move_base/global_costmap/obstacle_layer/parameter_descriptions
/move_base/global_costmap/obstacle_layer/parameter_updates
/move_base/global_costmap/parameter_descriptions
/move_base/global_costmap/parameter_updates
/move_base/global_costmap/static_layer/parameter_descriptions
/move_base/global_costmap/static_layer/parameter_updates
/move_base/goal
/move_base/local_costmap/costmap
/move_base/local_costmap/costmap_updates
/move_base/local_costmap/footprint
/move_base/local_costmap/inflation_layer/parameter_descriptions
/move_base/local_costmap/inflation_layer/parameter_updates
/move_base/local_costmap/obstacle_layer/clearing_endpoints
/move_base/local_costmap/obstacle_layer/parameter_descriptions
/move_base/local_costmap/obstacle_layer/parameter_updates
/move_base/local_costmap/parameter_descriptions
/move_base/local_costmap/parameter_updates
/move_base/parameter_descriptions
/move_base/parameter_updates
/move_base/result
/move_base/status
/move_base_simple/goal
```

## the action movebase

movebase is meant to behave as an action:

- you can send a 2D position to reach, topic `/move_base/goal` of type `move_base_msgs/MoveBaseAction`
- the fullfillment of the goal could require a lot of time
- finally, the move-base node will send a result message with the success flag set

for the complete list of interfaces opened by move-base, see [move_base_msgs package](http://docs.ros.org/en/fuerte/api/move_base_msgs/html/index-msg.html)

also [the official wiki](http://wiki.ros.org/move_base) is a good reference (very seldom, sometimes it happens...)

### A note about the action names

- *MoveBase.action* -- the name of the action, file with 3 fields

this message is split into 3 different messages:

- *MoveBaseActionGoal*
- *MoveBaseActionResult*
- *MoveBaseActionFeedback*

but *what you need are these names:* 

- *MoveBaseGoal*
- *MoveBaseResult*
- *MoveBaseFeedback*

note that in C++ you can refer to them in this way:

```c++
// include of the action
#include "move_base_msgs/MoveBaseAction.h"
// referring to the file MoveBase.action

// the goal will be
move_base_msgs::MoveBaseGoal goal;

// the feedback will be
move_base_msgs::MoveBaseFeedback feedback;

// and finally, the result will be
move_base_msgs::MoveBaseResult result;
```

### move_base_msgs/MoveBaseGoal

```
geometry_msgs/PoseStamped target_pose
```

### move_base_msgs/MoveBaseFeedback

```
geometry_msgs/PoseStamped base_position
```

### move_base_msgs/MoveBaseResult

**empty means that** the framework will sendo only a simple signal. 

```
# ... empty
```

## other interfaces

### geometry_msgs/PoseStamped

a pose with the recording time and *the frame*

```
# A Pose with reference coordinate frame and timestamp
Header header
Pose pose
```

**remember to give a frame for the pose!** if you don't know what to take as pose frame, use this:

```python
msg.goal.target_pose.header.frame_id = "map"
```

### geometry_msgs/Pose

a pose is made up of

- a position
- and a orientation *as quaternion*

```
# A representation of pose in free space, composed of postion and orientation. 
Point position
Quaternion orientation
```

**Gazebo spatial convention**

- horizontal plane : **x** (red) and **y** (green)
- vertical axis : **z** (blue)

**Rotation ROS convention**

see the answer under [this post](https://answers.ros.org/question/58863/incorrect-rollpitch-yaw-values-using-getrpy/)

- *x* -- roll
- *y* -- pitch
- *z* -- yaw

### geometry_msgs/Quaternion

representation of a rotation using 4 parameters. 

```
# This represents an orientation in free space in quaternion form.

float64 x
float64 y
float64 z
float64 w
```

**C++ how to convert quaternion to roll-pitch-yaw**

see [this simple example on GitHub Gist](https://gist.github.com/marcoarruda/f931232fe3490b7fa20dbb38da1195ac)

```c++
// tf contains some tool to deal with the most common geonetry for the Geometrical Unit
#include <tf/tf.h>

double roll, pitch, yaw;

tf::Quaternion q( x, y, z, w )
tf::Matrix3x3 m(q);
m.getRPY(roll, pitch, yaw);
```

### geometry_msgs/Point

```
# This contains the position of a point in free space
float64 x
float64 y
float64 z
```

## MoveBase action calls in C++

here are some templates if you wnat (like me) to put the hands on without bothering too much with technical details (geeky geeky).

### C++ -- headers

```C++
#include "actionlib/client/simple_action_client.h"
#include "actionlib/client/terminal_state.h"
#include "tf/tf.h"

#include <string>

#include "geometry_msgs/PoseStamped.h"
/*
std_msgs/Header header
geometry_msgs/Pose pose
*/

#include "geometry_msgs/Pose.h"
/*
Point position
Quaternion orientation
*/

#include "geometry_msgs/Quaternion.h"
/*
float64 x, y, z, w
*/

#include "geometry_msgs/Point.h"
/*
float x, y, z
*/

#include "move_base_msgs/MoveBaseAction.h"
// move_base_msgs::MoveBaseGoal
/*
geometry_msgs/PoseStamped target_pose
*/
// move_base_msgs::MoveBaseFeedback
/*
geometry_msgs/PoseStamped base_position
*/
// move_base_msgs::MoveBaseResult
	// empty
```

### C++ -- main with async spinner

**use async-spinner** for your node, in this way:

```
int main( int argc, char* argv[] )
{
	// ...
	
	ros::AsyncSpinner spinner( 2 );
	spinner.start( );
	
	// ...
	
	ros::waitForShutdown( ); // same as ros::spin( );
	
	return 0;
}
```

### C++ -- open the action client

**declaration of the macros and pointers**: declare globally

```C++
// move_base action client
#define ACTION_MOVE_BASE "move_base"
#define TIMEOUT_MOVE_BASE 5
actionlib::SimpleActionClient<move_base_msgs::MoveBaseAction> *actcl_move_base( );
```

**open the interface** in the main function, assuming the client global for the node, 

```C++
TLOG( "opening action client " << LOGSQUARE( ACTION_MOVE_BASE ) << " ... " );
actionlib::SimpleActionClient<move_base_msgs::MoveBaseAction> tactcl_move_base( ACTION_MOVE_BASE, true );
if( !tactcl_act.waitForServer( ros::Duration( TIMEOUT_MOVE_BASE ) ) )
{
	TERR( "unable to connect to the action server (timeout " << TIMEOUT_MOVE_BASE << "s) " 
		<< "-- action " << LOGSQUARE( ACTION_MovE_BASE ) << "\n"
		<< "\t " << (tactcl_move_base.isServerConnected( ) ? " it seems not online " : " service online ")  ) << "\n"
		<< "\t" << "STATUS: " << tactcl_move_base.getState( ).toString( ) );
	return 0;
}
actcl_move_base = &tactcl_move_base;
TLOG( "opening action client " << LOGSQUARE( ACTION_MOVE_BASE ) << " ... OK!" );
```

now the simple action client should be online. 

### C++ -- use the client -- callback based approach

see [waitForResult](https://docs.ros.org/en/diamondback/api/actionlib/html/classactionlib_1_1SimpleActionClient.html#a94b5a38fae6917f83331560b81eea341)

see also [sendGoal](https://docs.ros.org/en/diamondback/api/actionlib/html/classactionlib_1_1SimpleActionClient.html#ae6a2e6904495e7c20c59e96af0d86801)

very useful [this wiki page](http://wiki.ros.org/actionlib_tutorials/Tutorials/Writing%20a%20Callback%20Based%20Simple%20Action%20Client)

**sequential style pattern** if you just want to try something simple. 

```
// Called once when the goal becomes active
void cbk_active_move_base( )
{
	// ...
}

// feedback subscription
void cbk_feedback_move_base(
	const move_base_msgs::MoveBaseFeedback::ConstPtr& feedback )
{
	// ... access the feedback using -> operator
	
	// ...
}

// Called once when the goal completes
void cbk_done_move_base(
	const actionlib::SimpleClientGoalState& state,
	const move_base_msgs::MoveBaseResult& res )
{
	/*
	ROS_INFO( "Finished in state [%s]", state.toString( ).c_str( ) ) ;
	*/
	
	// ...
}

int send_goal_pos( float x, float y, float z )
{
	// ...
	
	move_base_msgs::MoveBaseGoal goal;
	
	// goal.target_pose.pose.{position.{x,y,z}, orientation.{x,y,z,w}}
	
	actcl_move_base->sendGoal( goal, 
		&cbk_done_move_base, 
		&cbk_active_move_base, 
		&cbk_feedback_move_base );
	
	// ...
}
```

**...but I don't want to use 3 callback, I use only 2/1 of them**. well, ActionLib defined three "placeholders":

see [the code](https://docs.ros.org/en/diamondback/api/actionlib/html/simple__action__client_8h_source.html#l00076)

```c++
// for the "done" callback
Client::SimpleDoneCallback( )

// for the "active" callback
Client::SimpleActiveCallback( )

// for the callback subscribing to the feedback topic
//    (if you don't need feedbacks)
Client::SimpleFeedbackCallback( )


// EXAMPLE: I don't need the done and the active
ac.sendGoal( goal, 
	Client::SimpleDoneCallback( ),
	Client::SimpleActiveCallback( ),
	&my_beautiful_feedback_callback
	);
```

**methods inside a class?** no problem: *use boost* as follows:

```c++
// the function
boost::bind( &MyClass::cbk_quello_che_vuoi, &MyClass, _1, _2 )
```

note that sometimes this call could return a very strange and unintelligible error: an example is contained [in this post](https://answers.ros.org/question/195723/actionlib-callback-based-simpleactionclient-bind-error/), which was a simple type. *but in my case* (my usual luck, nothing special) the error was due to the use of `const move_base_msgs::MoveBaseResult::ConstPtr& res` instead of the correct form `const move_base_msgs::MoveBaseResultConstPtr& res`.

**class implementation**

```c++
class move_base_interface
{
public:
	
	move_base_interface( ) : 
		actcl_move_base( "move_base", true ),
		running( false ), idle( true );
	{
		TLOG( "opening action client " << LOGSQUARE( ACTION_MOVE_BASE ) << " ... " );
		
		actcl_move_base.waitForServer( );
		if( !this->actcl_act.waitForServer( ros::Duration( TIMEOUT_MOVE_BASE ) ) )
		{
			TERR( "unable to connect to the action server (timeout " << TIMEOUT_MOVE_BASE << "s) " 
				<< "-- action " << LOGSQUARE( ACTION_MOVE_BASE ) << "\n"
				<< "\t " << (this->actcl_move_base.isServerConnected( ) ? " it seems not online " : " service online ")  ) << "\n"
				<< "\t" << "STATUS: " << this->actcl_move_base.getState( ).toString( ) );
			
			return;
		}
		
		TLOG( "opening action client " << LOGSQUARE( ACTION_MOVE_BASE ) << " ... OK!" );
	}
	
	// call the service
	bool send_goal( 
		move_base_msgs::MoveBaseGoal goal, 
		bool wait = false, 
		ros::Duration d = ros::Duration( TIMEOUT_MOVE_BASE ) )
	{
		
		// ...
		
		this->last_goal = goal;
		actcl_move_base.sendGoal( this->last_goal, 
			boost::bind(&move_base_interface::cbk_done_move_base, this, _1, _2 ),
			boost::bind(&move_base_interface::cbk_active_move_base, this ),
			boost::bind(&move_base_interface::cbk_feedback_move_base, this, _1 )
			);
		
		// ...
		
		if( wait )
		{
			if( !actcl_move_base.wait_for_results( d ) )
			{
				TERR( "action client for " << LOGSQUARE( ACTION_MOVE_BASE ) << "TIMEOUT EXPIRED " );
				actcl_move_base.cancelAllGoals( );
				
				// ...
				
				return false;
			}
		}
		
		// ...
		
		return true;
	}
	
	// Called once when the goal becomes active
	void cbk_active_move_base( )
	{
		this->idle = false;
		this->running = true;
		
		// ...
	}

	// feedback subscription
	void cbk_feedback_move_base(
		const move_base_msgs::MoveBaseFeedback::ConstPtr& feedback )
	{	
		// ... access the feedback using -> operator
		
		// ...
	}

	// Called once when the goal completes
	void cbk_done_move_base(
		const actionlib::SimpleClientGoalState& state,
		const move_base_msgs::MoveBaseResult& res )
	{
		this->idle = true;
		this->running = false;
		
		/*
		ROS_INFO( "Finished in state [%s]", state.toString( ).c_str( ) );
		*/
		
		// ...
	}
	
	// cancel the last request
	bool cancel( )
	{
		if( this->running )
		{
			actcl_move_base.cancelAllGoals( );
			
			this->idle = true;
			this->running = false;
			
			return true;
		}
		
		// idle! nothing to cancel
		return false;
	}
	
	/// check activity flag
	bool is_running( ) { return this->running; }
	
	/// check activity flag
	bool is_idle( ) { return this->idle; }
	
	/// action client status from the handle
	std::string get_state( ) { return actcl_move_base.getState.toString( ); }
	
private:
	
	/// the action client
	actionlib::SimpleActionClient<move_base_msgs::MoveBaseAction> actcl_move_base;
	
	/// the current goal
	move_base_msgs::MoveBaseGoal last_goal;
	
	/// activity flag
	bool running;
	
	/// idle flag
	bool idle;
};
```

## SLAM and GMapping -- navigation stack structure

infos about [GMapping](http://wiki.ros.org/gmapping)

see [sma_gmapping on GitHub](https://github.com/ros-perception/slam_gmapping)

see also the [tf package on ROS Wiki](http://wiki.ros.org/tf)

see also the [nav stack official page](http://wiki.ros.org/navigation)

[openSLAM official page](https://openslam-org.github.io/gmapping.html)

[Gazebo Plugins](https://classic.gazebosim.org/tutorials?tut=ros_gzplugins)

*laser-based simultaneous localization and mapping*

### Hokuyo laser range finder

ROS documentation about [hoyuko laser range finder](https://wiki.ros.org/hokuyo_node)

**publishers**:

- `/scan` : sensor_msgs/LaserScan

**Gazebo** simulation of this range sensor finder: RIMPIAZZA `hokuyo_link` nel codice col link che rappresenta il sensore nel file URDF. 

```xml
<!-- NON-GPU VERSION -->

<gazebo reference="hokuyo_link">
	<sensor type="ray" name="head_hokuyo_sensor">
		<pose>0 0 0 0 0 0</pose>
		<visualize>false</visualize>
		<update_rate>40</update_rate>
		
		<ray>
		<scan>
			<horizontal>
				<samples>720</samples>
				<resolution>1</resolution>
				<min_angle>-1.570796</min_angle>
				<max_angle>1.570796</max_angle>
			</horizontal>
		</scan>
		
		<range>
			<min>0.10</min>
			<max>30.0</max>
			<resolution>0.01</resolution>
		</range>
		
		<noise>
		<type>gaussian</type>
			<!-- Noise parameters based on published spec for Hokuyo laser
			achieving "+-30mm" accuracy at range < 10m.  A mean of 0.0m and
			stddev of 0.01m will put 99.7% of samples within 0.03m of the 
			true reading. -->
			<mean>0.0</mean>
			<stddev>0.01</stddev>
			</noise>
		</ray>
		
		<plugin name="gazebo_ros_head_hokuyo_controller" filename="libgazebo_ros_laser.so">
			<topicName>/scan</topicName>
			<frameName>hokuyo_link</frameName>
		</plugin>
	</sensor>
</gazebo>

<!-- GPU VERSION -->
<gazebo reference="hokuyo_link">
	<sensor type="gpu_ray" name="head_hokuyo_sensor">
		<pose>0 0 0 0 0 0</pose>
		<visualize>false</visualize>
		<update_rate>40</update_rate>
		
		<ray>
		<scan>
			<horizontal>
				<samples>720</samples>
				<resolution>1</resolution>
				<min_angle>-1.570796</min_angle>
				<max_angle>1.570796</max_angle>
			</horizontal>
		</scan>
		
		<range>
			<min>0.10</min>
			<max>30.0</max>
			<resolution>0.01</resolution>
		</range>
		
		<noise>
		<type>gaussian</type>
			<!-- Noise parameters based on published spec for Hokuyo laser
			achieving "+-30mm" accuracy at range < 10m.  A mean of 0.0m and
			stddev of 0.01m will put 99.7% of samples within 0.03m of the 
			true reading. -->
			<mean>0.0</mean>
			<stddev>0.01</stddev>
			</noise>
		</ray>
		
		<plugin name="gazebo_ros_head_hokuyo_controller" filename="libgazebo_ros_gpu_laser.so">
			<topicName>/scan</topicName>
			<frameName>hokuyo_link</frameName>
		</plugin>
	</sensor>
</gazebo>
```

NOta bene: dev'esserci un link di riferimento nel robot, quello dov'è piazzato il sensore. 

- il plugin pubblicherà la scansione sul topic `<topicName>/scan</topicName>`
- il link usato come origine del sensore è `<frameName>hokuyo_link</frameName>`, da cambiare a piacimento
- occhio anche al campo `<gazebo reference="hokuyo_link">`

## Usare MoveBase senza ricorrere alla actionlib

- per inviare il goal, usa `` di tipo ``
- per cancellare la richiesta, basta inviare un messaggio, *anche vuoto*, sul topic `` di tipo ``

### richiesta di cancellazione

basta pubblicare un messaggio di tipo `actionlib_msgs/GoalID.msg` sul topic `/move_base/cancel`, anche vuoto, per annullare l'ultima richiesta. 

```text
# rostopic info /move_base/cancel 
Type: actionlib_msgs/GoalID

Publishers: None

Subscribers: 
 * /move_base (http://3b17871017fd:39227/)


# rosmsg show actionlib_msgs/GoalID
time stamp
string id

```

### richiesta di movimento verso un target

pubblica la richiesta sul topic `/move_base/goal` di tipo `move_base_msgs/MoveBaseActionGoal`.

```text
# rostopic info /move_base/goal 
Type: move_base_msgs/MoveBaseActionGoal

Publishers: 
 * /move_base (http://3b17871017fd:39227/)

Subscribers: 
 * /move_base (http://3b17871017fd:39227/)


# rosmsg show move_base_msgs/MoveBaseActionGoal

std_msgs/Header header
  uint32 seq
  time stamp
  string frame_id
actionlib_msgs/GoalID goal_id
  time stamp
  string id
move_base_msgs/MoveBaseGoal goal
  geometry_msgs/PoseStamped target_pose
    std_msgs/Header header
      uint32 seq
      time stamp
      string frame_id
    geometry_msgs/Pose pose
      geometry_msgs/Point position
        float64 x
        float64 y
        float64 z
      geometry_msgs/Quaternion orientation
        float64 x
        float64 y
        float64 z
        float64 w

```

```{warning}
non basta semplicemente dire dove andare purtroppo. Move base sempluicemente non accetta il comando nel caso del semplice target.

affinchè move_base accetti il target, bisogna anche fornire il frame rispetto a cui il target è dato. 
```

vedi questo esempio di invio di richiesta a move_base (in c++, ma il concetto è lo stesso):

```c++
move_base_msgs::MoveBaseActionGoal goal;


// === VERY VERY VERY IMPORTANT === //

	goal.frame_id = "map";
	goal.goal.target_pose.header.frame_id = "map";

// === VERY VERY VERY IMPORTANT === //


goal.goal.target_pose.pose.position.x = x;
goal.goal.target_pose.pose.position.y = y;
goal.goal.target_pose.pose.position.z = z;
goal.goal.target_pose.pose.orientation.x = 0;
goal.goal.target_pose.pose.orientation.y = 0;
goal.goal.target_pose.pose.orientation.z = 0;
goal.goal.target_pose.pose.orientation.w = 1.0;
```
