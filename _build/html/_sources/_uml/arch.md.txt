
# UML -- Overall Architecture

---

```{uml}
---
Caption: Contents
---
./arch.md
```

---

## A simplified schema

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

cloud "RCL mission manager" as MISSION_MANAGER
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

## A more detailed schema

The project is a rich composition of many packages, resulting in a complex, heavy centralized architecture. It behaves as a classical hyerarchical architecture: sense, then plan, then act. 

```{uml}
@startuml

''' INFOS
title Project Architecture
skinparam Padding 8
allow_mixing


''' COMPONENTS
package armor{
database "aRMOR" as ARMOR
component "RCL aRMOR interface" as RCL_ARMOR

RCL_ARMOR -- ARMOR
}

package "RCL Mission Manager"{
component "RCL mission manager" as MISSION_MANAGER
component "RCL manipulation unit" as MANIP_UNIT
component "RCL navigation unit" as NAV_UNIT

MISSION_MANAGER -- MANIP_UNIT
}

package "RCL ROSPlan" {
component "ROSPlan pipeline manager" as ROSPLAN
component "RCL kb_tools" as RCL_KBTOOLS
database "ROSPlan knowledge base" as ROSPLANKB
component "ROSPlan components" as ROSPLAN_COMPONENTS
file "PDDL RCL domain" as DOMAIN
file "PDDL RCL problem" as PROBLEM
node "feedback manager" as FEEDBACK
package "RCL ROSPlan Actions" as ROSPLAN_ACTIONS {
node REPLAN
node SOLVE
node COLLECT
}
FEEDBACK --> ROSPLAN
ROSPLAN_ACTIONS --> FEEDBACK
ROSPLAN_COMPONENTS -- ROSPLAN_ACTIONS

ROSPLANKB-- ROSPLAN_COMPONENTS
ROSPLANKB -- RCL_KBTOOLS
ROSPLANKB -- ROSPLAN
RCL_KBTOOLS -- ROSPLAN
DOMAIN -- ROSPLANKB
PROBLEM -- ROSPLANKB
ROSPLAN_COMPONENTS -- ROSPLAN
}

package "erl3"{
component "erl3 Oracle" as ORACLE
component "markers publisher" as MARKERS
}

package "RCL Vision" {
component "RCL vision detection" as VISION
component "RCL vision decoder" as VISION_DECODE

VISION -- VISION_DECODE
}

package "Simulated Environment" as ENVIRONMENT {
component "RCL robot 'hunter'"
node "Gazebo"
node "RViz"
}

package moveit {
component "MoveIt!" as MOVEIT
}

package "navigation stack" as NAVSTACK {
component "Move Base"
component "AMCL"
component "SLAM/GMapping"
}

package "RCL Movement Controller" {
component "RCL navigation manager" as MOVEMENT
component "RCL manipulation controller" as MANIPULATION
package behaviours {
node "bug_m"
node "move_base_nav"
node "head_orientation"
node "go_to_point"
}
}

''' CONNECTIONS
MISSION_MANAGER -- RCL_ARMOR
MISSION_MANAGER -- VISION_DECODE
MISSION_MANAGER -- ROSPLAN
VISION_DECODE -- ORACLE
MARKERS -- NAV_UNIT
VISION -- ENVIRONMENT
ENVIRONMENT -- MOVEIT
ENVIRONMENT -- NAVSTACK
MOVEIT -- MANIPULATION
NAVSTACK -- MOVEMENT
MANIP_UNIT -- MANIPULATION
NAV_UNIT -- MOVEMENT
NAV_UNIT -- ROSPLAN_ACTIONS
ORACLE -- MISSION_MANAGER

@enduml
```

