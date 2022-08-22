
# UML -- bug_m PY node

---

```{toctree} 
./bug-m.md
```

---

## SCHEMA

```{uml} 
@startuml

''' INFOS
title NODE bug_m
skinparam Padding 8
allow_mixing


''' COMPONENTS
database "ROS parameter server" as PARAMETERS {
	node "des_pos_x"
	node "des_pos_y"
	node "des_yaw"
}

component "bug_m" <<ROSPy node>> as NODE_BUG_M {
	node "clbk_odom" as NODE_BUG_M.READ_ODOM
	node "clbk_laser" as NODE_BUG_M.LISTENER_LASER
	node "bug_switch" as NODE_BUG_M.SWITCH
}

NODE_BUG_M "get" <-- PARAMETERS

() "/odom" as TOPIC_ODOM
NODE_BUG_M.READ_ODOM "sub" <-- TOPIC_ODOM
note on link: nav_msgs/Odometry.msg

() "/scan" as TOPIC_LASER
NODE_BUG_M.LISTENER_LASER "sub" <-- TOPIC_LASER
note on link: sensor_msgs/LaserScan.msg

() "/cmd_vel" as TOPIC_VEL
NODE_BUG_M "pub" --> TOPIC_VEL
note on link: geometry_msgs/Twist.msg

() "/bug_m_signal" as TOPIC_BUG_M_SIGNAL
NODE_BUG_M "cl" <-- TOPIC_BUG_M_SIGNAL
note on link: std_srvs/Empty.srv

() "/go_to_point_switch" as SRV_GOTO_SWITCH
NODE_BUG_M "cl" <-- SRV_GOTO_SWITCH
note on link: std_srvs/SetBool.srv

/'
() "/wall_follower_switch" as SRV_WALL_SWITCH
NODE_BUG_M "cl" <-- SRV_WALL_SWITCH
note on link: std_srvs/SetBool.srv
'/

() "/head_orient_switch" as SRV_HEAD_ORIENT_SWITCH
NODE_BUG_M "cl" <-- SRV_HEAD_ORIENT_SWITCH
note on link: std_srvs/SetBool.srv

() "/bug_switch" as SRV_BUG_SWITCH
NODE_BUG_M.SWITCH "srv" --> SRV_BUG_SWITCH
note on link: std_srvs/SetBool.srv

@enduml
```

## HOW TO use the node -- basic way

it requires another node listening for the odom channel and checking when to deactivate the service

- set `des_pos_x`, `des_pos_y` and `des_yaw` into the paramater server
- activate the service with a request to `/bug_m_switch` of type `std_srvs/SetBool` (flag `SetBool.Request.data=True`)
- ... wait until the robot is near enough to the point
- turn off the node (flag `SetBool.Request.data=False`)

## HOW TO use the node -- user interface

the node can signal another node when the final position hasn't been reached. 

- set `des_pos_x`, `des_pos_y` and `des_yaw` into the paramater server
- activate the service with a request to `/bug_m_switch` of type `std_srvs/SetBool` (flag `SetBool.Request.data=True`)
- open a client with `/bug_m_signal` of type `std_srvs/Empty`
- ... wait ...
- when the robot is near to the target point, the node bug_m calls the service
	- from this, the node can infer that the node has reached the final position
