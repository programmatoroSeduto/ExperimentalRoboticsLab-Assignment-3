
# UML -- ROSPlan pipeline

## schema of ROSPlan

```{uml} 
@startuml

''' INFOS
title ROSPlan main pipeline
skinparam Padding 10
allow_mixing


''' COMPONENTS
file "robolcuedo\nproblem" <<PDDL>> as PROBLEM
file "robolcuedo\ndomain" <<PDDL>> as DOMAIN

package "ROSPlan"{
  database "Knowledge Base" <<ROSPlan>> as RP_KB
  component "Problem Interface" <<ROSPlan>> as RP_PROBLEM
  component "Planning Interface" <<ROSPlan>> as RP_PLAN
  component "Plan Parsing" <<ROSPlan>> as RP_PARSE
  component "Plan Dispatcher" <<ROSPlan>> as RP_DISPATCH
  
  RP_KB -right-> RP_PROBLEM
  RP_PROBLEM --> RP_PLAN
  RP_PLAN --> RP_PARSE
  RP_KB --> RP_PARSE
  RP_PARSE --> RP_DISPATCH
  RP_KB --> RP_DISPATCH
}

''' INTERFACES
() "/rosplan_problem_interface/problem_generation_server" as SRV_PROBLEM_TRIG
RP_PROBLEM "srv" -right-> SRV_PROBLEM_TRIG
note on link : std_srvs/Empty

() "/rosplan_planner_interface/planning_server" as SRV_PLAN_TRIG
RP_PLAN "srv" -right-> SRV_PLAN_TRIG
note on link : std_srvs/Empty

() "/rosplan_parsing_interface/parse_plan" as SRV_PARSE_TRIG
RP_PARSE "srv" -right-> SRV_PARSE_TRIG
note on link : std_srvs/Empty

() "/rosplan_plan_dispatcher/dispatch_plan" as SRV_DISPATCH_TRIG
RP_DISPATCH "srv" -right-> SRV_DISPATCH_TRIG
note on link : rosplan_dispatch_msgs/DispatchService

''' CONNECTIONS
RP_KB <-up- PROBLEM
RP_KB <-up- DOMAIN

@enduml
```

message **rosplan_dispatch_msgs/DispatchService** : 

```text
---
bool success
bool goal_achieved
```

## SCHEMA -- rosplan_pipeline_manager

```{uml} 
@startuml

''' INFOS
title ROSPlan main pipeline
skinparam Padding 10
allow_mixing


''' COMPONENTS
component "rosplan \n pipeline manager" <<RoboCLuedo>> as RCL_PIPELINE

''' INTERFACES
() "/rosplan_problem_interface/problem_generation_server" as SRV_PROBLEM_TRIG
RCL_PIPELINE "cl" <-- SRV_PROBLEM_TRIG
note on link : std_srvs/Empty

() "/rosplan_planner_interface/planning_server" as SRV_PLAN_TRIG
RCL_PIPELINE "cl" <-- SRV_PLAN_TRIG
note on link : std_srvs/Empty

() "rosservice call /rosplan_parsing_interface/parse_plan" as SRV_PARSE_TRIG
RCL_PIPELINE "cl" <-- SRV_PARSE_TRIG
note on link : std_srvs/Empty

() "/rosplan_plan_dispatcher/dispatch_plan" as SRV_DISPATCH_TRIG
RCL_PIPELINE "cl" <-- SRV_DISPATCH_TRIG
note on link : rosplan_dispatch_msgs/DispatchService

() "/update_goal" as SRV_UPDATE_GOAL
RCL_PIPELINE "cl" <-- SRV_UPDATE_GOAL
note on link : robocluedo_rosplan_msgs/UpdateGoal

() "/robocluedo/action_feedback" as TOPIC_ACT_FEEDBACK
RCL_PIPELINE "sub" <-left- TOPIC_ACT_FEEDBACK
note on link : robocluedo_rosplan_msgs/ActionFeedback

() "/rosplan_problem_interface/problem_instance" as TOPIC_PROBLEM_INSTANCE
RCL_PIPELINE "sub" <-- TOPIC_PROBLEM_INSTANCE
note on link : std_msgs/String

() "/robocluedo/pipeline_manager" as SRV_PIPELINE
RCL_PIPELINE "srv" -right-> SRV_PIPELINE
note on link : robocluedo_rosplan_msgs/RosplanPipelineManagerService

''' CONNECTIONS


@enduml
```

## SCHEMA -- feedback manager

```{uml}
@startuml

''' INFOS
title ROSPlan feedback manager
skinparam Padding 10
allow_mixing


''' COMPONENTS
class "Feedback Manager" as FEEDBACK
{
  + fb_???( ...params... )
}

() "/robocluedo/action_feedback" as SRV_FEEDBACK
FEEDBACK --> SRV_FEEDBACK
note on link: robocluedo_rosplan_msgs/ActionFeedback


''' CONNECTIONS
' ...

@enduml
```
