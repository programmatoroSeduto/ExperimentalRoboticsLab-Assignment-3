
# UML -- robocluedo rosplan -- in a nutshell

---

```{toctree}
---
caption: Contents
---
./rosplan-arch.md
```

---

## elements

nodes and components:

- knowledge base
- problem interface
- planning interface
- parsing interface
- plan dispatcher 

... actions ...

- feedback manager
- rosplan kb interface
- rosplan pipeline manager

## UML -- Architecture (simplified)

here's a essential representation of the package plus the ROSPlan system, controlled by this package:

```{uml} 
@startuml

''' DIAGRAM INFOS
title "robocluedo rosplan"
skinparam Padding 8


''' DIAGRAM ELEMENTS
package "robocluedo rosplan" {

file "pddl\ndomain" as PDDL_DOM
file "pddl\nproblem" as PDDL_PROB

package "ROSplan" {
	database "knowledge base" <<ROSPlan>> as KB
	[problem interface] <<ROSPlan>> as PROBLEM
	[planning interface] <<ROSPlan>> as PLAN
	[parsing interface] <<ROSPlan>> as PARSE
	[plan dispatcher] <<ROSPlan>> as DISPATCH

	KB -right-- PROBLEM
	PROBLEM -down-- PLAN
	PLAN -down-- PARSE
	KB -right- PARSE
	PARSE -down-- DISPATCH
	KB -left- DISPATCH
}

package "actions dispatch" {
	[action] as ACTION
}

[feedback\nmanager] as FEEDBACK
[ROSPlan\npipeline manager] <<node>> as MANAGER
[KB interface] <<node>> as IKB
ACTION --- IKB

DISPATCH -right-- ACTION
PDDL_DOM --- KB
PDDL_PROB --- KB
IKB --- KB

ACTION --- FEEDBACK

PROBLEM -right-- MANAGER
PLAN -right-- MANAGER
PARSE -right-- MANAGER
DISPATCH -right-- MANAGER
}

() "feedback topic" as TOPIC_FEEDBACK
FEEDBACK --> TOPIC_FEEDBACK
() "service pipeline manager" as SRV_MANAGER
MANAGER --> SRV_MANAGER
() "KB interface service" as SRV_IKB
IKB --> SRV_IKB
@enduml
```

### Blocks

let's see better some important blocks:

- the **rosplan pipeline manager** controls the steps the ROSPlan architecture uses to make and execute the plan, from the problem loading phase to the dispatch. Another fundamental functionality provided by this block is to make explainable possible problems that could occur during the planning, with a rich feedback send to the node using the pipeline manager. 

- the service for dispatching the plan provides just two informations: if the plan has been executed successfully, and if the goal has been fulfilled at the end of the dispatch, and unfortunately *nothing else*. Here's where the **feedback manager** comes in: role of the feedback manager is to make the ROSPlan actions able to explain in details each possible "problematic" situation which could happen during the plan dispatch. The action encountering something wrong *publishes* a precise type of feedback through a topic reserved for the feedback exchange, and the ROSPlan pipeline manager exploits such a feedback to explain what's wrong with the last plan dispatch. 

- another fundamental tool inside the package is the so-called **KB interface**, which provides the ability to modify the predicates of the problem instance inside the knowledge base. This eposes services making easier to *replan* as well as to *alter* and to *read* the informations inside the knowledge base. Its implementation follows the particular characteristics of the PDDL domain, hence one of its roles is to *hide the spcificity of the PDDL problem*, providing a general, flexible interface for handling the problem in a (kinda) "standard" way. 

- the dispach can interact with the system using a bunch of both ROS actions and Simulated Actions, enabling the system to perform functionalities such as navigation and manipulation. 
