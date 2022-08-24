
# Oracle2 -- User Documentation

```{seealso}
See [*CarmineD8/erl2* on GitHub](https://github.com/CarmineD8/erl2.git)
```

---

```{toctree}
./erl2-oracle-user-manual.md
```

---

purposes of this node are

- to choose a solution of the mystery
- to provide hints to the system in order to enable it inferring the solution
- to check the solution, as a sort of arbiter of the game
- to provide the posision of the hint sources and to visualize them in RViz

## SCHEMA -- node *simulation.cpp*

```{uml} 
@startuml

''' INFOS
title (erl2) node simulation.cpp
skinparam Padding 8
allow_mixing

component "simulation.cpp" <<ERL Oracle>> as ORACLE {
	node "oracleCallback" as ORACLE.ORACLE_CALLBACK
	' to trigger the message, the link "cluedo_link" must be at a distance up to 0.25m
}

note left of ORACLE.ORACLE_CALLBACK
	to trigger the message, the link "cluedo_link" must be at a distance up to 0.25m
end note

() "/visualization_marker" as SRV_MARKER
ORACLE "pub" ..> SRV_MARKER
note on link: visualization_msgs/MarkerArray.msg

() "/oracle_solution" as SRV_ORACLE_SOLUTION
ORACLE "srv" --> SRV_ORACLE_SOLUTION
note on link: erl2/oracle.srv

() "/gazebo/link_states" as TOPIC_GAZEBO_LINKS
ORACLE.ORACLE_CALLBACK "sub" <.. TOPIC_GAZEBO_LINKS
note on link: gazebo_msgs/LinkStates.msg

() "/oracle_hint" as TOPIC_HINT
ORACLE.ORACLE_CALLBACK "pub" --> TOPIC_HINT
note on link: erl2/ErlOracle.msg

@enduml
```

### message *erl2::ErlOracle*

this message is sent every time the **cluedo_link** is at a distance up to *0.25m*. 

```yaml
int32 ID
string key
string value
```

*the message could be corrupted* in many ways:

- empty field(s)
- one or more fields could be "-1"

so, *remember to check the validity of the message*.

### service *erl2::Oracle*

this service is used to check the validity of a solution in "solve" phase. it returns *the winner ID* choosed at the beginning of the game by the Oracle. 

the system should check wether the ID of the solution is equal to the one returned by this service. *the returned value is never corrupted.*

```yaml
---
int32 ID
```

note: in the actual implementation, the ID goes from 0 included to 5 included. 

### message *visualization_msgs::MarkerArray* 

each marker represents 

- a operation (add marker, remove marker, ...)
- *a marker with the graphical interface*

message structure:

```yaml
 Marker[] markers
```

### message *visualization_msgs::Marker*

how the node creates markers:

```c++ 
// visualization_msgs::Marker marker;

// header
marker.header.frame_id = "odom";
marker.header.stamp = ros::Time();
// operation type
marker.action = visualization_msgs::Marker::ADD;

// position
marker.pose.position.x = 0.0;
marker.pose.position.y = 0.0;
marker.pose.position.z = 0.0;

// orientation
marker.pose.orientation.x = 0.0;
marker.pose.orientation.y = 0.0;
marker.pose.orientation.z = 0.0;
marker.pose.orientation.w = 1.0;

// scale
marker.scale.x = 0.5;
marker.scale.y = 0.5;
marker.scale.z = 0.5;

// shape and color (RGBA)
marker.type = visualization_msgs::Marker::SPHERE;
marker.color.r = 0.0;
marker.color.g = 1.0;
marker.color.b = 0.0;
marker.color.a = 1.0; 
```

the structure of the message:

```yaml
# See http://www.ros.org/wiki/rviz/DisplayTypes/Marker and http://www.ros.org/wiki/rviz/Tutorials/Markers%3A%20Basic%20Shapes for more information on using this message with rviz

uint8 ARROW=0
uint8 CUBE=1
uint8 SPHERE=2
uint8 CYLINDER=3
uint8 LINE_STRIP=4
uint8 LINE_LIST=5
uint8 CUBE_LIST=6
uint8 SPHERE_LIST=7
uint8 POINTS=8
uint8 TEXT_VIEW_FACING=9
uint8 MESH_RESOURCE=10
uint8 TRIANGLE_LIST=11

uint8 ADD=0
uint8 MODIFY=0
uint8 DELETE=2
uint8 DELETEALL=3

Header header                        # header for time/frame information
string ns                            # Namespace to place this object in... used in conjunction with id to create a unique name for the object
int32 id                           # object ID useful in conjunction with the namespace for manipulating and deleting the object later
int32 type                         # Type of object
int32 action                         # 0 add/modify an object, 1 (deprecated), 2 deletes an object, 3 deletes all objects
geometry_msgs/Pose pose                 # Pose of the object
geometry_msgs/Vector3 scale             # Scale of the object 1,1,1 means default (usually 1 meter square)
std_msgs/ColorRGBA color             # Color [0.0-1.0]
duration lifetime                    # How long the object should last before being automatically deleted.  0 means forever
bool frame_locked                    # If this marker should be frame-locked, i.e. retransformed into its frame every timestep

#Only used if the type specified has some use for them (eg. POINTS, LINE_STRIP, ...)
geometry_msgs/Point[] points
#Only used if the type specified has some use for them (eg. POINTS, LINE_STRIP, ...)
#number of colors must either be 0 or equal to the number of points
#NOTE: alpha is not yet used
std_msgs/ColorRGBA[] colors

# NOTE: only used for text markers
string text

# NOTE: only used for MESH_RESOURCE markers
string mesh_resource
bool mesh_use_embedded_materials
```

## HOW TO receive a hint

**NOTE: THE URDF MODEL MUST HAVE A LINK LABELED 'cluedo_link'; THE ORACLE WILL TAKE ITS POSITION IN ORDER TO UNDERSTAND WHERE TO SEND THE HINT VIA TOPIC.**

place the *cluedo_link* near to the manipulator. Done that, thenode will receive a message from the topic `/oracle_hint` of type `erl2/ErlOracle.msg`.


## HOW TO check the solution

first of all, the system should *infer* the solution *as integer ID*. 

then, send a request to the service `/oracle_solution` of type `erl2/oracle.msg`.


## Development settings

the source code `simulation.cpp` exposes two important macros:

- `NOT_TESTING` -- when set to `false`, the Oracle will publish only the hints referred to the `winID` i.e. the selected winner ID. 
	
	it allows to make quicker the testing phase, because you haven't unvalid hints from the Oracle, as well as several different IDs to manage.
	
	use this setting for testing only. the macro should be set to `true`. 
	
- `DIST_THRESH` -- the oracle sends a hint only when the `cleudo_link` of the robot has a distance from the marker which is lower than this value. 
	
	the default value is 0.25
	
	by increasing it, it's possible to receive a hint remaining a bit far from the marker, and over 0.35, to receive hints also when the robot simply passes in front of if
