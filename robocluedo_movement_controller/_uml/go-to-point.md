
# UML -- go to point PY node

```{toctree} 
./go-to-point.md
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
	node "des_pos_x"
	node "des_pos_y"
}

component "go_to_point_service" <<ROSPy node>> as NODE_GO_TO_POINT {
	node "go_to_point_switch" as NODE_GO_TO_POINT.GOTOPOINT_SWITCH
	node "clbk_odom" as NODE_GO_TO_POINT.READ_ODOM
	
}

NODE_GO_TO_POINT "get" <-- PARAMETERS

() "/cmd_vel" as TOPIC_VEL
NODE_GO_TO_POINT "pub" --> TOPIC_VEL
note on link: geometry_msgs/Twist.msg

() "/odom" as TOPIC_ODOM
NODE_GO_TO_POINT.READ_ODOM "sub" <-- TOPIC_ODOM
note on link: nav_msgs/Odometry.msg

() "/go_to_point_switch" as SRV_GOTO_SWITCH
NODE_GO_TO_POINT.GOTOPOINT_SWITCH "srv" --> SRV_GOTO_SWITCH
note on link: std_srvs/SetBool.srv

@enduml
```

## HOW it works

the node ccan be used only when *there's no obstacle between the robot and the target to reach*. here's how it works:

- turn the head of the robot and orient it towards the target
- *go straight* until the target hasn't been reached

## HOW TO use the node -- basic way

it requires another node listening for the odom channel and checking when to deactivate the service

- set `des_pos_x` and `des_pos_y` into the paramater server
- activate the service with a request to `/go_to_point_switch` of type `std_srvs/SetBool` (flag `SetBool.Request.data=True`)
- ... wait until the robot is near enough to the point
- turn off the node (flag `SetBool.Request.data=False`)
