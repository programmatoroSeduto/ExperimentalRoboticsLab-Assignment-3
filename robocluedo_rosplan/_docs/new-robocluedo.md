
# PDDL Design -- the new RoboCLuedo PDDL model

```{warning}
This document is very old, and for sure it needs to be updated. You can find the current model in the folder `/robocluedo_rosplan/pddl/robocluedo_3`.

I leave this document here because it contains the very first design process of the current PDDL, which is changed a lot during the development of the project. Here are the basic principles. 
```

```{todo}
a big half of this document is still *in italian*. Remember to translate it. 
```

---

```{toctree}
---
caption: Contents
maxdepth: 3
---
./new-robocluedo.md
```

---

## Differences from the previous version

see [the previous version of the RoboCLuedo #2 Project](https://github.com/programmatoroSeduto/ExperimentalRoboticsLab-Assignment-2-OLD.git)

- usage of *landmarks* instead of only one goal
	
	a landmark is a sort of *checkpoint*, and in particular it will correspond to a particular phase of the mission. The AI will will work on two levels: the highest one, the *mission level*, will decide how to combine landmarks in order to let the mission succeed, whereas the lowest one, implemented using rosplan, plans how to carry out the single action depending on the current status of the robot. 
	
	For instance, the mission manager could say "COLLECT A HINT", leavng the planner the task to decide precisely which sequence of actions the robot has to take in order to fulfill the current mission objective. 
	
- the PDDL just manages the movements and how the robot works, and not other informations. Clear distinction between the KB os ROSPlan and the aRMOR KB
	
	in the previous version of the model, the rosplan KB has been used also as a common KB for the reasoning. This requires to declare predicates inside the PDDL domain, and to decide how to combine two sets of informations. It is a very poor design choice, which lets to a overcomplicated implementation. *It is simpler to separate the true reasoning from the simple problem solving*. 
	
- *a simpler, concise model*: this increases the flexibility of the model, and in particular it makes quicker any future update of the arch
	
- much more like a stata machine than the previous version
	
- *no usage of fluents*
	
	in this case, using fluents is not so important. In order to keep as simpler as possible the declaration of the problem, it is better to rely only on the simple predicates.



## A list of Landmarks

```{note}
the implementation of the landmarks is a bit more cumbersome, even if the principle is the one outlined here. See {any}`kb_interface` for more informations. 
```

Here are the macro-objectives the mission planner could require to the ROSPlan unit:

- **replan**
	preconditions:
	```lisp
	(dirty )
	```
	goal:
	```lisp
	(not-dirty )
	```
- **propose a solution -- solve**
	preconditions:
	```lisp
	(not-dirty )
	```
	goal:
	```lisp
	(dirty )
	```
- **collect a new hint -- collect**
	preconditions:
	```lisp
	(not-dirty )
	```
	goal:
	```lisp
	(not-dirty )
	(hint-collected ?wp )
	```



## list of actions

```{note}
This document reports the very first version of the code, *which is sometimes not correct*. Please refer to the models inside the folder `/robocluedo_rosplan/pddl/robocluedo_3`.
```

```{note}
*document in italian* (thinking in english makes everything ... annoyingly slower)
```

### REPLAN

- **(ACTION) replan**
	- c'è un bool `(dirty )` che indica quando il sistema non è inizializzato
	- questa azione serve solo per "resettare" la KB di rosplan
	- per l'aggiornamento, la action usa il nodo `KB interface` che si occuperà di lettura e scrittura nella kb di rosplan

azione PDDL:

```lisp
(:durative-action replan
	:parameters ( )
	:duration (= ?duration 0.99 )
	:condition (and 
		(at start (dirty ))
	)
	:effect (and 
		(at end (not (dirty )))
		(at end (not-dirty ))
	)
)
```

implementazione ROS:

```{uml} 
@startuml

''' DIAGRAM INFOS
title "(ACTION) replan"
skinparam Padding 8
allow_mixing


''' COMPONENTS
component "replan" <<PDDL ROS action>> as ACT_REPLAN


''' LINKS
() "/replan" as SRV_IKB
ACT_REPLAN "cl" <-- SRV_IKB
note on link: std_msgs::Empty

() "action dispatch" as DISPATCH
ACT_REPLAN <.. DISPATCH
() "action feedback" as DISP_FEEDBACK 
ACT_REPLAN ..> DISP_FEEDBACK

() "/robocluedo/action_feedback" as TOPIC_FEEDBACK_MANAGER
ACT_REPLAN "pub" --> TOPIC_FEEDBACK_MANAGER
note on link: robocluedo_rosplan_msgs/ActionFeedback

@enduml
```

### COLLECT

- **(ACTION) move-to**
	- semplice azione di movimento da un punto ad un altro
	- il sistema è *molto simile a quello della vecchia versione* se non per qualche predicato di meno
	- le posizioni in cui il robot può andare sono quelle non marcate come `(is-center ?wp)`, ovvero usa il not
	- il robot *non può compiere un'azione di movimento verso lo stesso punto in cui si trova*: insomma, deve muoversi
	- l'azione si aspetta che qualche altro sistema implementi un servizio a cui l'azione si connette come client per interagire col navigation system
	- tale servizio si occupa anche di associare il nome del waypoint alla posizione del waypoint in termini di coordinate cartesiane

azione PDDL:

```lisp
(:durative-action move-to
	:parameters ( ?from ?to - waypoint )
	
	:duration (= ?duration 0.99 )
	
	:condition (and
		;;status
		(at start (not-hint-ready ))
		(at start (not-dirty ))
		
		;; localization
		(at start (at ?from ))
		
		;; passage
		(at start (passage ?from ?to ))
		
		;; path tracking
		(at start (not-explored ?to ))
		
		;;center conditions
		(at start (not-is-center ?to ))
	)
	
	:effect (and
		;; localization
		(at start (not (at ?from )))
		(at end (at ?to ))
		
		;; (in any case)
		(at end (not (at-center )))
		(at end (not-at-center ))
		
		;; path tracking
		(at start (not (not-explored ?to)))
		(at start (explored ?to))
	)
)
```

implementazione ROS:

```{uml} 
@startuml

''' DIAGRAM INFOS
title "(ACTION) move_to"
skinparam Padding 8
allow_mixing


''' COMPONENTS
component "move-to" <<PDDL action>> as ACT_MOVE_TO


''' LINKS
() "/robocluedo/action_feedback" as TOPIC_FEEDBACK_MANAGER
ACT_MOVE_TO "pub" --> TOPIC_FEEDBACK_MANAGER
note on link: robocluedo_rosplan_msgs/ActionFeedback

() "/robocluedo/navigation_command" as SRV_NAV
ACT_MOVE_TO "srv" --> SRV_NAV
note on link: robocluedo_rosplan_msgs/NavigationCommand

() "action dispatch" as DISPATCH
ACT_MOVE_TO <.. DISPATCH
() "action feedback" as DISP_FEEDBACK 
ACT_MOVE_TO ..> DISP_FEEDBACK

@enduml
```

--- 

- **(ACTION) signal-collect-hint**
	- il robot si prepara a raccogliere l'hint dal marker
	- **implementato come una simulated action**

azione PDDL:

```lisp
(:durative-action signal-collect-hint 
	:parameters ( ?wp - waypoint )
	
	:duration (= ?duration 0.99 )
	
	:condition (and
		;; status
		(at start (not-hint-ready ))
		(at start (not-dirty ))
		
		;; localization
		(at start (at ?wp ))
		
		;; hint inside
		(at start (not-hint-collected ?wp ))
	)
	
	:effect (and
		(at start (not (not-hint-ready )))
		(at end (hint-ready ))
	)
)
```

--- 

- **(ACTION) collect-hint**
	- l'azione implementa solo il moto del braccio verso il marker: avvicina, rimani fermo, rientra il braccio, e avanti
	- *in caso si presenterà il bisogno di aggiornare il sistema di raccolta degli hint* (ad esempio, detection degli hint tramite una videocamera piuttosto che tramite un sistema di hints) *si dovrà cambiare solo questa particolare azione* e stop. 
	- l'azione si aspetta che un altro nodo implementi il servizio che permette all'architettura di interagire con il manipulation controller

azione PDDL:

```lisp
(:durative-action collect-hint
	:parameters ( ?wp - waypoint )
	
	:duration (= ?duration 0.99 )
	
	:condition (and
		;; status
		(at start (hint-ready ))
		
		;; localization
		(at start (at ?wp ))
		(at start (not-is-center ?wp ))
	)
	
	:effect (and
		;; status
		(at end (not (hint-ready )))
		(at end (not-hint-ready ))
	)
)
```

implementazione ROS:

```{uml} 
@startuml

''' DIAGRAM INFOS
title "(ACTION) collect-hint"
skinparam Padding 8
allow_mixing


''' COMPONENTS
component "collect-hint" <<PDDL action>> as ACT_COLLECT_HINT


''' LINKS
() "/robocluedo/manipulator_command" as SRV_MANIP
ACT_COLLECT_HINT "cl" <-- SRV_MANIP
note on link : robocluedo_rosplan_msgs/ManipulationCommand

() "action dispatch" as DISPATCH
ACT_COLLECT_HINT <.. DISPATCH
() "action feedback" as DISP_FEEDBACK 
ACT_COLLECT_HINT ..> DISP_FEEDBACK

@enduml
```

### SOLVE

- **(ACTION) move-to-center** 
	- come precondizione il robot non si trova al centro, quindi la posizione attuale è `(not-is-center ?wp)`
	- al termine dell'azione, il robot è in stato `(ready-to-solve )`

azione PDDL:

```lisp
(:durative-action move-to-center
	:parameters ( ?from ?to - waypoint )
	
	:duration (= ?duration 0.99 )
	
	:condition (and
		;;status
		(at start (not-hint-ready ))
		(at start (not-dirty ))
		
		;; localization
		(at start (at ?from ))
		(at start (is-center ?to ))
		
		;; center conditions
		(at start (not-at-center ))
		
		;; passage
		(at start (passage ?from ?to ))
	)
	
	:effect (and
		;; localization
		(at start (not (at ?from )))
		(at end (at ?to ))
		
		;; in center
		(at start (not (not-at-center )))
		(at end (at-center ))
	)
)
```

implementazione ROS:

```{uml} 
@startuml

''' DIAGRAM INFOS
title "(ACTION) move-to-center"
skinparam Padding 8
allow_mixing


''' COMPONENTS
component "move-to-center" <<PDDL action>> as ACT_MOVE_TO_CENTER


''' LINKS
() "/robocluedo/action_feedback" as TOPIC_FEEDBACK_MANAGER
ACT_MOVE_TO_CENTER "pub" --> TOPIC_FEEDBACK_MANAGER
note on link: robocluedo_rosplan_msgs/ActionFeedback

() "/robocluedo/navigation_command" as SRV_NAV
ACT_MOVE_TO_CENTER "srv" --> SRV_NAV
note on link: robocluedo_rosplan_msgs/NavigationCommand

() "action dispatch" as DISPATCH
ACT_MOVE_TO_CENTER <.. DISPATCH
() "action feedback" as DISP_FEEDBACK 
ACT_MOVE_TO_CENTER ..> DISP_FEEDBACK

@enduml
```

--- 

- **(ACTION) solve** 
	- l'azione PDDL non fa che settare tutto per fare il replan
	- a differenza della precedente versione, il PDDL non tiene traccia delle ipotesi (per questo c'è aRMOR)
	- arrivato alla solve, il mission manager ricerca e propone la soluzione
	- **è implementata come una simulated action**

azione PDDL:

```lisp
(:durative-action solve
	:parameters ( )
	
	:duration (= ?duration 0.99 )
	
	:condition (and
		;;status
		(at start (not-hint-ready ))
		(at start (at-center ))
		(at start (not-dirty ))
		
	)
	
	:effect (and
		(at start (not (not-dirty )))
		(at end (dirty ))
	)
)
```

