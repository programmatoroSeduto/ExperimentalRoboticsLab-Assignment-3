
# UML -- robocluedo rosplan

## simple description

nodes and components:

- knowledge base
- problem interface
- planning interface
- parsing interface
- plan dispatcher

- ... actions ...

- feedback manager
- rosplan kb interface
- rosplan pipeline manager

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

	KB -right-> PROBLEM
	PROBLEM -down-> PLAN
	PLAN -down-> PARSE
	KB -right- PARSE
	PARSE -down-> DISPATCH
	KB -left- DISPATCH
}

package "actions dispatch" {
	[action] as ACTION
}

[feedback\nmanager] as FEEDBACK
[ROSPlan\npipeline manager] <<node>> as MANAGER
[KB interface] <<node>> as IKB
ACTION --> IKB

DISPATCH -right-> ACTION
PDDL_DOM --> KB
PDDL_PROB --> KB
IKB <--> KB

ACTION --> FEEDBACK

PROBLEM -right-> MANAGER
PLAN -right-> MANAGER
PARSE -right-> MANAGER
DISPATCH -right-> MANAGER
}

() "feedback topic" as TOPIC_FEEDBACK
FEEDBACK --> TOPIC_FEEDBACK
() "service pipeline manager" as SRV_MANAGER
MANAGER --> SRV_MANAGER
() "KB interface service" as SRV_IKB
IKB --> SRV_IKB
@enduml
```
