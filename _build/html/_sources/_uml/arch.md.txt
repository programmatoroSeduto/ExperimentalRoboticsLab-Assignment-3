
# UML -- Overall Architecture

```{uml}
@startuml

''' INFOS
title Project Architecture
skinparam Padding 8
allow_mixing


''' COMPONENTS
database "aRMOR" as ARMOR
component "MoveIt!" as MOVEIT
component "Navigation Stack" as NAVSTACK
component "erl3 Oracle" as ORACLE
component "ROSPlan" as ROSPLAN

component "RCL mission manager" as MISSION_MANAGER
component "RCL Vision" as VISION
component "RCL aRMOR interface" as RCL_ARMOR
component "RCL ROSPlan" as RCL_ROSPLAN
component "RCL Movement Controller" as MOVEMENT
component "RCL hunter environment" as ENVIRONMENT


''' CONNECTIONS
ENVIRONMENT <--> NAVSTACK
ENVIRONMENT <--> MOVEIT
ENVIRONMENT <--> VISION
NAVSTACK <--> MOVEMENT
MOVEIT <--> MOVEMENT
MOVEMENT <--> MISSION_MANAGER
MISSION_MANAGER <--> RCL_ROSPLAN
ROSPLAN <--> RCL_ROSPLAN
MISSION_MANAGER <--> RCL_ARMOR
ARMOR <--> RCL_ARMOR
MISSION_MANAGER <--> VISION
VISION <--> ORACLE

@enduml
```
