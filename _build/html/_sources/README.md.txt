
# README -- the RoboCLuedo Project -- ROUND 3

*Francesco Ganci* - S4143910 - A.A.2021/2022

---

> Compatible With:
> - ROS1 noetic
> 
> Documentation:
> - [Official Documentation for ERL3](erl3)
> 
> **Project Setup**:
> - see the [Official Docs](erl3setup)
> 
> **Run the project**:
> - see the [Official Documentation](erl3run)
> 
> Starter kit:
> - [CarmineD8/exp_assignment3 on GitHub](https://github.com/CarmineD8/exp_assignment3/)
> 
> Previous versions of the RCL project:
> - [programmatoroSeduto/ExperimentalRoboticsLab-Assignment-2 (Official Release) on GitHub](https://github.com/programmatoroSeduto/ExperimentalRoboticsLab-Assignment-2)
> - [programmatoroSeduto/ExperimentalRoboticsLab-Assignment-2-OLD (old version) on GitHub](https://github.com/programmatoroSeduto/ExperimentalRoboticsLab-Assignment-2-OLD.git)
> - [programmatoroSeduto/ExperimentalRoboticsLab-Assignment-1 on GitHub](https://github.com/programmatoroSeduto/ExperimentalRoboticsLab-Assignment-1.git)
> 
> In a hurry?
> - [See it in action!](videoexample)
> 

---

![Cluedo3](_media/cluedorickmorty.jpg)

---

## What you can find in this readme

This readme goes through the design ideas under the project. 

- expected behaviour of the project
- main design ideas
- Software Architecture components overview
- Software Architecture communication protocols overview

if you have more practical questions, you can find everything you want, detailed ad libitum, inside the [Official Documentation](officialdocs).

## Introduction -- RCL#3

The [RCL#3](https://github.com/programmatoroSeduto/ExperimentalRoboticsLab-Assignment-3) project, end of a long journey, wants to implement the architecture of a robot, using its sensors (in this case, cameras and a LIDAR sensor) to solve a *Cluedo*-style mystery, collecting hints across the environment and reasoning on them for finding the killer. The robot moves in the surroundings, trying to find as many ArUco IDs as possible, adding information to a OWL-based ontology for finding the solution of the case. 

### Technical Goals -- RCL#3

The main technical aim of this project is to *extend the architecture roposed in the third part of the project* with as the less adaptation work as possible. Indeed this version of the project keeps almost unchancged a lot of components from [RCL#2](https://github.com/programmatoroSeduto/ExperimentalRoboticsLab-Assignment-2), ad also something that comes from the [RCL#1](https://github.com/programmatoroSeduto/ExperimentalRoboticsLab-Assignment-1.git) project. 

Moreover, as in the other assignments, a great care has been given to *the metodology of work*: not only writing the code, but also using and developing its documentation during the work. A big effort has been made as well, in documenting about the technologies involved in the implementation of this assignment, trying to find a way to make it work which could work also on other platforms as much as possible. Finally, the project comes with a rich *module testing* allowing to ensure the functioning of each module, either single or in combination with other elements of the architecture. See the official project documentation for further informations. 

### Differences from RCL#2

As said above, there are few, but important, technical dfferences between the two versions of the project:

- a new robot model, including 4 cameras and a brand new set of poses
- a new navigation system, with a new controller integrating **move_base** and the navigation stack
- support for "offline" localisation using **AMCL**
- a brand new **ArUco** vision system
- updates on the manipulation controller, supporting (a very raw) asynchronous manipulation
- a brand new system for launching the components of the architecture
- minimal adaptation of the ROSPlan package to cope with the cpecificities of ERL#3

And other little differences around in the code and in the structure of the project. A big effort have been made in making the project execution *as much fast as possible*: using such a sensing system, the perfomrance becomes a important topic to deal with. 

## Expected Behaviour -- RCL#3

Here's how the robot should behave, just as a short overview of how the RCL#3 project works. 

### ERL#3 Environment

The robot starts in the middle of a small house with a number of hints (arUco markers) spread out in the space (at least 5 per room). The robot can see the markers and detect them. Here's how the environment appears on Gazebo after the project has been run:

![erl3-environment](_media/erl3-environment.png)

And here how it appears on RViz; the green balls are the positions of the rooms, called *waypoints*. The center waypoint, not indicated by a green marker, is located at the position where the robot starts the mission. 

![erl3-env-rviz](_media/erl3-env-rviz.png)

### Behaviour

The robot **moves around looking for markers to detect**. Each marker has a identifier, which, after a request to the Oracle node, enables the system to access a hint to add to the Ontology. The hint will be used for reasoning about the plausible solutions of the case, trying to distinguish between che "COMPLETE" ones and the "INCONSISTENT" ones. 

Starting from a location, the robot navigates to another waypoint, using the cameras to "see" the markers slong the way. Arrived at the waypoint, the robot asks for admissible hypotheses to the Ontology. If at least one hypothesis is found acceptable, the robot navigates to the center and, once arrived there, the system introduces its solution to the Oracle node, which checks if the solution is either right or wrong. 

If the proposal is wrong, the robot continues to explore in search of other hints, after having discarded the proposed hypothesis. Otherwise, the robot ends working. 

The robot keeps going from one waypoint to another one, collecting hints until a plausible solution is available. 

## SW Arch -- Components

The project is a rich composition of many packages, resulting in a complex, heavy centralized architecture. It behaves as a classical hyerarchical architecture: sense, then plan, then act.

### highlights of the architecture

Here are the concept I used during each phase of the job:

- **separation** : Great care has been given in *guaranteeing the neat separation between components* 
	
	for instance, the RCL ROSPlan package doesn't interact directly with the motion controllers: it exposes a client to implement instead, which is called each time a ROSPlan action needs to perform the navigation. It requires a *bridge node*, that is a delegate between the ROSPlan framework and the real navigation system. 

- **abstraction** : the interfaces provided by each package *aims at hiding the implementation details*, enhancing a standard, intuitive way to use each package
	
	in particular, most of them provide a number of *directives*, i.e. spcecific operations an external node expects to be implemented in a way that the node don't want to know. The aRMOR package along with this project is a good example of this. 

- **flexibility and adaptability** : the neat separation between packages, alongside with the abstraction, lets the architecture to be more flexible and easily updatable. 

- **module testing** : it is a fundamental concept traversing all the lifecycle of this project since RCL#1
	
	*Each time you think about some code, also think to how to test it*. The project contains a rich module testing section, allowing the developer to not wasting time in looking for bugs in a very complex, demanding architecture. Each package can be tested independently, and most of the times with just a bash script. Sometimes also some code has been provided in order to test the project.
	
	*Developing a rich debug system is a necessity* with such a large project. You cannot go on if you can't be sure at 100% that the components "at the bottom" work as expected. It helped me during the development phase hundreds of times, even revealing bugs whicc was very difficult to spot simply running the proejct.

### Components -- simplified schema

It highlights the centralized structure of the architecture. Each component here corresponds to a package used by the project. 

The mission manager contains the bridges above-mentioned (there are bridges for both the navigation and the manipulation), as well as the *main node* providing a high-layer planning by means of landmarks and aRMOR operations. 

```{uml}
@startuml

''' INFOS
title Project Architecture
skinparam Padding 10
allow_mixing


''' COMPONENTS
database "aRMOR" as ARMOR
component "MoveIt!" as MOVEIT
component "Navigation Stack" as NAVSTACK
component "erl3 Oracle" as ORACLE
component "ROSPlan" as ROSPLAN

node "RCL mission manager" as MISSION_MANAGER
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
MISSION_MANAGER -- ORACLE

@enduml
```

---

### Components -- extended schema 

> 
> Wanna see more? Take a look at the packages documentation inside the Official Documentation. 
> 

This schema wants to give an idea of how much the architecture of the project is extended. Inside it there are pointed out many ideas inside RCL#3:

- the ROSPlan side is a sort of mikrokosmos of components. The system uses the idea of *landmarks*, implementing the "low level planning system". See the official documentation, and in particular the design of the PDDL, for further details about such a planning policy. 
	
	The problem solution strategy is divided into three "steps", called *landmakrs* in the terminology of the project:
	
	- **REPLAN** : tidy the rosplan kb up, maing it ready for a new iteration of the strategy
	- **COLLECT** : move the robot in the environment, *using a topological localisation* integrated with the ROSPlan PDDL model
	- **SOLVE** : the final step, introduce the "assumed" solution of the case and check it; the robot moves towards the center before introducing the solution

- the movement controller package implements both the navigation system and the manipulation system. 
	
	the navigation system is divided into many layers (hierarchical architecture inside): the behavioural section, containing 
	
	- the *elementary behaviours* directly influencing the motion of the robot, 
	- combined into *composite behaviours* which group the elementary behaviours to make more structured motion strategies
	- and the *motion controllers*, components of the navigation manager (implemented as classes inside that node) providing a "cozy" abstraction for the navigation system: a external node sees onl a simple interface, without having the need to deal with the details of the implementation of the navigation system.
	
- the diagram also shows the components of the navigation stack, a novelty of RCL#3

Here's the diagram (I suggest you to open the image in a new tab of the browser... it is quite big)

```{uml}
@startuml

''' INFOS
title Project Architecture
''' skinparam Padding 8
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
MISSION_MANAGER -- ORACLE
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

@enduml
```

---

## SW Arch -- communication protocols overview

Here are the temporal diagrams of the project, describing just the essential steps performed in communicating between packages

### navigation using move_base (RCL#3)

**package temporal diagram**: 

```{uml}
@startuml

title RCL#3 move_base navigation - package temporal diagram

''' ENTITIES 
collections ROSPLAN
collections MISSION
collections MOVEMENT

''' SEQUENCE
' == section ==
' ...
== First setup (first request) ==

MISSION -> MOVEMENT : SET controller ID=1
MISSION <- MOVEMENT : success
hnote over MISSION: waiting for markers

== Movement ==

ROSPLAN -> MISSION : request from ROSPlan action
MISSION -> MOVEMENT : navigation request
hnote over MOVEMENT : performing navigation
MISSION <- MOVEMENT : target reached
ROSPLAN <- MISSION : navigation ended

@enduml
```

**node temporal diagram**: 

```{uml}
@startuml

title RCL#3 move_base navigation - node temporal diagram
' header ...
' footer ...

''' ENTITIES 
participant MOVE_TO order 0
participant NAVIGATION_UNIT order 1
participant NAVIGATION_MANAGER order 2
participant NAVIGATION_CONTROLLER order 3
participant MOVE_BASE_NAV order 5
participant HEAD_ORIENTATION order 6
participant MOVE_BASE order 5

''' SEQUENCE
' == section ==
' ...

== request ==
MOVE_TO -> NAVIGATION_UNIT : using RCL ROSPlan interface
hnote over NAVIGATION_UNIT 
	waiting for markers
end hnote
NAVIGATION_UNIT -> NAVIGATION_MANAGER : using RCL navigation req
note over NAVIGATION_MANAGER
	let's assume a controller 
	has been activated before...
	otherwise, the service returns 
	immediately
end note
NAVIGATION_MANAGER --> NAVIGATION_CONTROLLER : passing request (call)


== navigation job ==
NAVIGATION_CONTROLLER -> MOVE_BASE_NAV : switch ON
hnote over NAVIGATION_CONTROLLER
	waiting for a signal ...
end hnote
note over MOVE_BASE_NAV
	the behaviour receives 
	a final orientation to achieve
	and a target to reach
end note
MOVE_BASE_NAV -> MOVE_BASE : send goal
hnote over MOVE_BASE_NAV
	reading distance error ...
end hnote
MOVE_BASE_NAV -> MOVE_BASE : cancel goal
MOVE_BASE_NAV -> HEAD_ORIENTATION : switch ON
hnote over MOVE_BASE_NAV
	reading orientation error ...
end hnote
MOVE_BASE_NAV -> HEAD_ORIENTATION : switch OFF
NAVIGATION_CONTROLLER <- MOVE_BASE_NAV : send signal
NAVIGATION_CONTROLLER -> MOVE_BASE_NAV : switch OFF

== response ==
NAVIGATION_MANAGER <-- NAVIGATION_CONTROLLER : return
NAVIGATION_UNIT <- NAVIGATION_MANAGER : response (RCL movement controller)
MOVE_TO <- NAVIGATION_UNIT : response (RCL ROSPlan)

@enduml
```

### navigation using bug_m (RCL#2)

```{uml}
@startuml

title RCL#3 bug_m navigation - package temporal diagram

''' ENTITIES 
collections ROSPLAN
collections MISSION
collections MOVEMENT

''' SEQUENCE
' == section ==
' ...
== First setup (first request) ==

MISSION -> MOVEMENT : SET controller <b>ID=0</b>
MISSION <- MOVEMENT : <i>success</i>
hnote over MISSION: waiting for markers

== Movement ==

ROSPLAN -> MISSION : request from ROSPlan action
MISSION -> MOVEMENT : navigation request
hnote over MOVEMENT 
performing navigation 
with BUG_M behaviour
end hnote
MISSION <- MOVEMENT : target reached
ROSPLAN <- MISSION : navigation ended

@enduml
```

### manipulation (RCL#2) -- synchronous manipulation

```{uml}
@startuml

title RCL#3 move_base navigation - node temporal diagram
' header ...
' footer ...

''' ENTITIES 
participant MOVE_TO order 0
participant NAVIGATION_UNIT order 1
participant NAVIGATION_MANAGER order 2
participant NAVIGATION_CONTROLLER order 3
participant BUGM_NAV order 5
participant HEAD_ORIENT order 6
participant GOTO_POINT order 5

''' SEQUENCE
' == section ==
' ...

== request ==
MOVE_TO -> NAVIGATION_UNIT : using RCL ROSPlan interface
hnote over NAVIGATION_UNIT 
	waiting for markers
end hnote
NAVIGATION_UNIT -> NAVIGATION_MANAGER : using RCL navigation req
note over NAVIGATION_MANAGER
	let's assume a controller 
	has been activated before...
	otherwise, the service returns 
	immediately
end note
NAVIGATION_MANAGER --> NAVIGATION_CONTROLLER : call function


== navigation job ==
NAVIGATION_CONTROLLER -> BUGM_NAV : switch ON
hnote over NAVIGATION_CONTROLLER
	waiting for a signal ...
end hnote
note over BUGM_NAV
	the behaviour receives 
	a final orientation to achieve
	and a target to reach
end note

note over BUGM_NAV
here the BUGM_NAV Could 
perform a backwards movement
for gaining manouver space
end note

BUGM_NAV -> GOTO_POINT : switch ON

hnote over BUGM_NAV
	reading position error ...
end hnote

hnote over GOTO_POINT
turning 
towards the target ...
end hnote

hnote over GOTO_POINT
navigating straight 
to the target
end hnote

BUGM_NAV -> GOTO_POINT : switch OFF

BUGM_NAV -> HEAD_ORIENT : switch ON

hnote over BUGM_NAV
	reading orientation error ...
end hnote

hnote over HEAD_ORIENT
	turning
end hnote

BUGM_NAV -> HEAD_ORIENT : switch OFF

NAVIGATION_CONTROLLER <- BUGM_NAV : send signal
NAVIGATION_CONTROLLER -> BUGM_NAV : switch OFF

== response ==
NAVIGATION_MANAGER <-- NAVIGATION_CONTROLLER : return
NAVIGATION_UNIT <- NAVIGATION_MANAGER : response (RCL movement controller)
MOVE_TO <- NAVIGATION_UNIT : response (RCL ROSPlan)

@enduml
```

### manipulation (RCL#3) -- asynchrononus manipulation

```{uml}
@startuml

title ...
header ...
footer ...

''' ENTITIES 
' partecipant "" as ...
' components  "" as ...
' ...

''' SEQUENCE
' == section ==
' ...

@enduml
```

### Mission manager workflow

```{uml}
@startuml

title ...
header ...
footer ...

''' ENTITIES 
' partecipant "" as ...
' components  "" as ...
' ...

''' SEQUENCE
' == section ==
' ...

@enduml
```
