
# UML -- RoboCLuedo Vision UML diagram

This diagram shows the vision system as a "pipeline" starting from the signals out of the cameras of the robot, and passing through a VISION_DETECT phase and a VISION_DECODE phase. The system publishes the hints observed from the scene. 

```{uml}
@startuml

title RoboCLuedo Vision UML diagram
left to right direction

''' ENTITIES
component VISION_DETECT
component VISION_DECODE
component ORACLE
package ROBOT {
node CAM_ARM
node CAM_LEFT
node CAM_RIGHT
node CAM_FRONT

CAM_ARM -[hidden]> CAM_LEFT
CAM_LEFT -[hidden]> CAM_RIGHT
CAM_RIGHT -[hidden]> CAM_FRONT
}

() "/robocluedo_robot/camera_arm/image_raw" as TOPIC_ARM
() "/robocluedo_robot/camera_front/image_raw" as TOPIC_FRONT
() "/robocluedo_robot/camera_left/image_raw" as TOPIC_LEFT
() "/robocluedo_robot/camera_right/image_raw" as TOPIC_RIGHT

CAM_ARM "pub" --> TOPIC_ARM
CAM_LEFT "pub" --> TOPIC_LEFT
CAM_RIGHT "pub" --> TOPIC_RIGHT
CAM_FRONT "pub" --> TOPIC_FRONT

TOPIC_ARM --> "sub" VISION_DETECT
TOPIC_FRONT --> "sub" VISION_DETECT
TOPIC_LEFT --> "sub" VISION_DETECT
TOPIC_RIGHT --> "sub" VISION_DETECT

() "/aruco_detected_ids" as TOPIC_IDS
VISION_DETECT --> TOPIC_IDS : std_msgs/Int32.msg

TOPIC_IDS --> "sub" VISION_DECODE

() "oracle_hint_original" as SRV_REAL_ORACLE
ORACLE "srv" --> SRV_REAL_ORACLE : exp_assignment3/Marker.srv

SRV_REAL_ORACLE --> "cl" VISION_DECODE

() "/oracle_hint" as TOPIC_ORACLE
VISION_DECODE "pub" --> TOPIC_ORACLE : erl2/ErlOracle.msg

@enduml
```
