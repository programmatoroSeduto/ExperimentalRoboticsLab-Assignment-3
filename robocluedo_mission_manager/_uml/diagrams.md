
# UML -- mission manager interfaces

---

```{toctree}
---
caption: Contents
---
./diagrams.md
```

---

This document provides a reference of all the interfaces provided by the `robocluedo_mission_manager` package. The most complex node from this point of view is the `mission_manager.cpp` which is the "head" of the entire architecture. 

## The mission manager

There are many interfaces involving this node. Let's see it under many points of view.

### mission_manager -- rosplan communication

```{uml}
@startuml

title Mission Manager -- ROSPlan communication
left to right direction

''' COMPONENTS
component MISSION_MANAGER <<RCL Mission Manager>>
component PIPELINE <<RCL ROSPlan>>

''' INTERFACES
() "/robocluedo/pipeline_manager" as SRV_PIPELINE
PIPELINE "srv" --> SRV_PIPELINE
note on link: robocluedo_rosplan_msgs/RosplanPipelineManagerService.srv

''' LINKS
SRV_PIPELINE --> "cl" MISSION_MANAGER

@enduml
```

### mission_manager -- continuous manipulation

```{uml}
@startuml

title Mission Manager -- continuous manipulation
left to right direction

''' COMPONENTS
component MISSION_MANAGER <<RCL Mission Manager>>
component MANIPULATION_UNIT <<RCL Mission Manager>>

''' INTERFACES
() "/tip_pos_auto_switch" as SRV_MANIP_UNIT
MANIPULATION_UNIT "srv" --> SRV_MANIP_UNIT
note on link: std_srvs/SetBool.srv

''' LINKS
SRV_MANIP_UNIT --> "cl" MISSION_MANAGER

@enduml
```

### mission_manager -- sensing

```{uml}
@startuml

title Mission Manager -- sensing
left to right direction

''' COMPONENTS
component MISSION_MANAGER <<RCL Mission Manager>>
component VISION_DECODE <<RCL Vision>>

''' INTERFACES
() "/oracle_hint" as TOPIC_ORACLE_HINT
VISION_DECODE "pub" --> TOPIC_ORACLE_HINT
note on link: std_srvs/SetBool.srv

''' LINKS
TOPIC_ORACLE_HINT --> "sub" MISSION_MANAGER

@enduml
```

### mission_manager -- interaction with aRMOR interface

```{uml}
@startuml

title Mission Manager -- interaction with aRMOR interface

''' COMPONENTS
component MISSION_MANAGER <<RCL Mission Manager>>
component ARMOR_INTERFACE <<RCL aRMOR>>

''' INTERFACES
together {
left to right direction
() "/cluedo_armor/add_hint" as SRV_ARMOR_ADD
() "/cluedo_armor/find_consistent_h" as SRV_ARMOR_FIND
() "/cluedo_armor/wrong_hypothesis" as SRV_ARMOR_DEL
() "/cluedo_armor/backup" as SRV_ARMOR_BACKUP
}

SRV_ARMOR_ADD <-left- "srv" ARMOR_INTERFACE
note on link: robocluedo_armor_msgs/AddHint.srv

SRV_ARMOR_FIND <-left- "srv" ARMOR_INTERFACE
note on link: robocluedo_armor_msgs/FindConsistentHypotheses.srv


SRV_ARMOR_DEL <-left- "srv" ARMOR_INTERFACE
note on link: robocluedo_armor_msgs/DiscardHypothesis.srv


SRV_ARMOR_BACKUP <-left- "srv" ARMOR_INTERFACE
note on link: std_srvs/Trigger.srv

''' LINKS
SRV_ARMOR_ADD --> "cl" MISSION_MANAGER
SRV_ARMOR_FIND --> "cl" MISSION_MANAGER
SRV_ARMOR_DEL --> "cl" MISSION_MANAGER
SRV_ARMOR_BACKUP --> "cl" MISSION_MANAGER


@enduml
```

### mission_manager -- communication with the Oracle

```{uml}
@startuml

title Mission Manager -- oracle
left to right direction

''' COMPONENTS
component MISSION_MANAGER <<RCL Mission Manager>>
component ORACLE <<erl_assignment3>>

''' INTERFACES
() "/oralce_solution" as SRV_ORACLE_SOLUTION
ORACLE "srv" --> SRV_ORACLE_SOLUTION
note on link: erl2/Oracle.srv

''' LINKS
SRV_ORACLE_SOLUTION --> "cl" MISSION_MANAGER

@enduml
```

## navigation_unit

```{uml}
@startuml

title Navigation Unit
left to right direction

''' COMPONENTS
component NAV_UNIT <<RCL Mission Manager>>
component NAV_MANAGER <<RCL Movement Controller>>
component MOVE_TO <<RCL ROSPlan Action>>
component MARKERS_PUBLISHER <<erl_assignment3>>

''' INTERFACES
() "/robocluedo/navigation_command" as SRV_ROSPLAN_NAV
NAV_UNIT "srv" --> SRV_ROSPLAN_NAV
note on link: robocluedo_rosplan_msgs/NavigationCommand.srv

() "/navigation_manager/navigation" as SRV_NAV
NAV_MANAGER "srv" --> SRV_NAV
note on link: robocluedo_movement_controller_msgs/NavigationService.srv

() "/navigation_manager/set_algorithm" as SRV_ALGO
NAV_MANAGER "srv" --> SRV_ALGO
note on link: robocluedo_movement_controller_msgs/Algorithm.srv

() "/visualization_marker" as TOPIC_MARKER
TOPIC_MARKER <-up- "pub" MARKERS_PUBLISHER
note on link: visualization_msgs/MarkerArray.msg

''' LINKS
SRV_ROSPLAN_NAV --> "cl" MOVE_TO
SRV_NAV --> "cl" NAV_UNIT
SRV_ALGO --> "cl" NAV_UNIT
TOPIC_MARKER -down-> "sub" NAV_UNIT

@enduml
```

## manipulation_unit

```{uml}
@startuml

title Navigation Unit
left to right direction

''' COMPONENTS
component MISSION_MANAGER <<RCL Mission Manager>>
component MANIP_UNIT <<RCL Mission Manager>>
component MANIP_CONTROLLER <<RCL Movement Controller>>

''' INTERFACES
/'
() "/tip_pos" as SRV_MANIP
MANIP_CONTROLLER "srv" --> SRV_MANIP
note on link: robocluedo_movement_controller_msgs/ManipulatorPosition.srv
SRV_MANIP --> "cl" MANIP_UNIT
'/

() "/tip_pos_auto_switch" as SRV_MANIP_RANDOM
MANIP_UNIT "srv" --> SRV_MANIP_RANDOM
note on link: std_srvs/SetBool.srv

() "/tip_pos_async" as TOPIC_MANIP_ASYNC
MANIP_UNIT "pub" --> TOPIC_MANIP_ASYNC
note on link: robocluedo_movement_controller_msgs/ManipulatorPositionAsync.srv

''' LINKS
SRV_MANIP_RANDOM --> "cl" MISSION_MANAGER
TOPIC_MANIP_ASYNC --> "sub" MANIP_CONTROLLER

@enduml
```
