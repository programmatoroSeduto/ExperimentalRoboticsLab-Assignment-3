
# UML -- move_base_nav

---

```{toctree} 
---
caption: Contents
maxdepth: 3
---
./move-base-nav.md
```

---

The node manages the navigation with move_base and the navigation stack, and coordinates move_base with the head orientation behaviour. 

## SCHEMA

```{uml} 
@startuml

''' INFOS
title NODE move_base_nav
skinparam Padding 8
allow_mixing


''' COMPONENTS
database "ROS parameter server" as PARAMETERS {
	node "des_pos_x"
	node "des_pos_y"
}

component "move_base_nav" <<ROSPy node>> as MOVE_BASE_NAV {
	node "move_base_nav_switch" as MOVE_BASE_NAV.SWITCH
	node "clbk_odom" as MOVE_BASE_NAV.READ_ODOM
	
}

MOVE_BASE_NAV "get" <-- PARAMETERS

() "/odom" as TOPIC_ODOM
MOVE_BASE_NAV.READ_ODOM "sub" <-- TOPIC_ODOM
note on link: nav_msgs/Odometry.msg

() "/cmd_vel" as TOPIC_VEL
MOVE_BASE_NAV "pub" --> TOPIC_VEL
note on link: geometry_msgs/Twist.msg

() "/move_base/goal" as TOPIC_MOVE_BASE_GOAL
MOVE_BASE_NAV "pub" --> TOPIC_MOVE_BASE_GOAL
note on link: move_base_msgs/MoveBaseActionGoal.msg

() "/move_base/cancel" as TOPIC_MOVE_BASE_CANCEL
MOVE_BASE_NAV "pub" --> TOPIC_MOVE_BASE_CANCEL
note on link: actionlib_msgs/GoalID.msg

() "/nav_stack_switch" as SRV_MOVE_BASE_NAV_SWITCH
MOVE_BASE_NAV.SWITCH "srv" --> SRV_MOVE_BASE_NAV_SWITCH
note on link: std_srvs/SetBool.srv

() "/nav_stack_signal" as SRV_MOVE_BASE_NAV_SIGNAL
MOVE_BASE_NAV <-- "cl" SRV_MOVE_BASE_NAV_SIGNAL
note on link: std_srvs/Empty.srv

() "/head_orient_switch" as SRV_HEAD_ORIENT_SWITCH
MOVE_BASE_NAV <-- "cl" SRV_HEAD_ORIENT_SWITCH
note on link: std_srvs/SetBool.srv

@enduml
```
