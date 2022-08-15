
# UML -- head orientation PY

```{toctree} 
./head-orient.md
```

## SCHEMA

```{uml} 
@startuml

''' INFOS
title NODE go_to_point
skinparam Padding 8
allow_mixing


''' COMPONENTS
database "ROS parameter server" as PARAMETERS {
	node "des_yaw"
}

component "head_orientation" <<ROSPy node>> as NODE_HEAD_ORIENT {
	node "head_orient_switch" as NODE_HEAD_ORIENT.SWITCH
	node "clbk_odom" as NODE_HEAD_ORIENT.READ_ODOM
	
}

NODE_HEAD_ORIENT "get" <-- PARAMETERS

() "/cmd_vel" as TOPIC_VEL
NODE_HEAD_ORIENT "pub" --> TOPIC_VEL
note on link: geometry_msgs/Twist.msg

() "/odom" as TOPIC_ODOM
NODE_HEAD_ORIENT.READ_ODOM "sub" <-- TOPIC_ODOM
note on link: nav_msgs/Odometry.msg

() "/head_orient_switch" as SRV_HEAD_ORIENT_SWITCH
NODE_HEAD_ORIENT.SWITCH "srv" --> SRV_HEAD_ORIENT_SWITCH
note on link: std_srvs/SetBool.srv

@enduml
```

## HOW TO use the node -- basic way

it requires another node listening for the odom channel and checking when to deactivate the service

- set `des_yaw` into the paramater server
- activate the service with a request to `/head_orient_switch` of type `std_srvs/SetBool` (flag `SetBool.Request.data=True`)
- ... wait until the error is under your threshold ...
- turn off the node (flag `SetBool.Request.data=False`)
