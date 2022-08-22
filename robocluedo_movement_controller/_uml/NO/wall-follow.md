
# UML -- wall follow PY node

```{toctree} 
./wall-follow.md
```

## SCHEMA

```{uml} 
@startuml

''' INFOS
title NODE wall_follow_service
skinparam Padding 8
allow_mixing


''' COMPONENTS
component "wall_follow_service" <<ROSPy node>> as NODE_WALL_FOLLOW {
	node "wall_follower_switch" as NODE_WALL_FOLLOW.WALL_FOLLOW_SWITCH
	node "clbk_laser" as NODE_WALL_FOLLOW.LISTENER_LASER
}

() "/cmd_vel" as TOPIC_VEL
NODE_WALL_FOLLOW "pub" --> TOPIC_VEL
note on link: geometry_msgs/Twist.msg

() "/scan" as TOPIC_LASER
NODE_WALL_FOLLOW.LISTENER_LASER "sub" <-- TOPIC_LASER
note on link: sensor_msgs/LaserScan.msg

() "/wall_follower_switch" as SRV_WALL_SWITCH
NODE_WALL_FOLLOW.WALL_FOLLOW_SWITCH "srv" --> SRV_WALL_SWITCH
note on link: std_srvs/SetBool.srv

@enduml
```

## HOW it works

the node, when active, makes the robot to go along the wall *in counter-clockwise direction*. 

- find a wall
- follow it
	- if there's no longer a wall to follow, turn left until the robot isn't able to find the wall, then continue following it
- always *go straight along the wall*
	- until the node is active

## HOW TO activate or deactivate the node

the node can be neabled or disabled using the service `wall_follower_switch` of type `std_srvs/SetBool`. the service returns immediately.

the flag `SetBool.Request.data` will be the new activity state of the node
