(define (domain robocluedo)

(:requirements 
	;; === PDDL1.1 === ;;
	:strips 
	:typing 
	:equality 
	:existential-preconditions
	:universal-preconditions 
	:conditional-effects
	:quantified-preconditions
	
	;; === PDDL2.1 === ;;
	:numeric-fluents
	:durative-actions
)

(:types
	;; ontology
	hypID
	
	;; navigation system
	waypoint
	
	;; hypothesis elements
	who
	where
	what
)

(:constants
	center - waypoint
)

(:predicates
	;; working status
	(pre-init )
	(investigating )
	(ready-to-acquire )
	(acquiring )
	(acquired )
	(thinking )
	(wanna-solve )
	(who-killed-doctor-black )
	(elementary-whatson )
	
	;; environment
	(passage ?w1 ?w2 - waypoint )
	
	;; navigation system
	(not-explored ?w - waypoint )
	(robot-position ?w - waypoint )
	
	;; manipulator
	(manipulator-off )
	(manipulator-on )
	
	;; hint gathering
	(not-acquired-hint ?wp - waypoint )
	(acquired-hint ?wp - waypoint )
	(can-give-hint ?wp - waypoint )
	
	;; hypotheses classification
	(h-open ?id - hypID )
	(h-complete ?id - hypID )
	(h-discard ?id - hypID )
	
	;; hypothesis elements
	(hyp-who ?id - hypID ?who - who)
	(hyp-where ?id - hypID ?where - where)
	(hyp-what ?id - hypID ?what - what)
)

(:functions
	(number-of-ids-in-the-problem )
	(remaining-moves )
	(solution-quality )
	
	;; hypotheses classification
	(h-count-who ?id - hypID )
	(h-count-where ?id - hypID )
	(h-count-what ?id - hypID )
	
	;; hypothesis counting
	(h-count-open )
	(h-count-complete )
	(h-count-discard )
)

;; === SYSTEM INIT === ;;

(:durative-action init-planning-system
	:parameters ( )
	
	:duration (= ?duration 1)
	
	:condition (at start (pre-init ))
	
	:effect (and
		(at start (not (pre-init )))
		(at end (investigating ))
	)
)

;; === NAVIGATION SYSTEM === ;;

(:durative-action move-to
	:parameters ( ?from ?to - waypoint )
	
	:duration (= ?duration 1)
	
	:condition (and
		;; status
		(at start (investigating ))
		
		;; localization
		(at start (robot-position ?from ))
		
		;; passage
		(at start (passage ?from ?to ))
		
		;; path tracking
		(at start (not-explored ?to ))
		
		;; manipulator
		(at start (manipulator-off ))
		
		;; it must be a new hint there
		; (at start (can-give-hint ?to ))
		; (at start (not-acquired-hint ?to ))
	)
	
	:effect (and
		;; localization
		(at start (not (robot-position ?from )))
		(at end (robot-position ?to ))
		
		;; path tracking
		(at end (not (not-explored ?to )))
		(at end (not-explored ?from ))
	)
)

(:durative-action move-to-center
	:parameters ( ?from - waypoint )
	
	:duration (= ?duration 1)
	
	:condition (and
		;; status
		(at start (wanna-solve ))
		
		;; localization
		(at start (robot-position ?from ))
		
		;; moves
		(at start (= (remaining-moves ) 0))
	)
	
	:effect (and
		;; localization
		(at start (not (robot-position ?from )))
		(at end (robot-position center))
		
		;; system status
		(at start (not (wanna-solve )))
		(at end (who-killed-doctor-black ))
	)
)

;; === MANIPULATOR === ;;

(:durative-action manipulator-near-marker
	:parameters ( ?wp - waypoint )
	
	:duration (= ?duration 1)
	
	:condition (and
		;; robot status
		(at start (ready-to-acquire ))
		
		;; position
		(at start (robot-position ?wp ))
		
		;; it must be a new hint there
		(at start (can-give-hint ?wp ))
		(at start (not-acquired-hint ?wp ))
		
		;; manipulator
		(at start (manipulator-off ))
	)
	
	:effect (and
		;; robot status
		(at start (not (ready-to-acquire )))
		(at end (acquiring ))
		
		;; manipulator status
		(at start (not (manipulator-off )))
		(at end (manipulator-on ))
	)
)

(:durative-action manipulator-far-marker
	:parameters ( ?wp - waypoint )
	
	:duration (= ?duration 1)
	
	:condition (and
		;; robot status
		(at start (acquired ))
		
		;; position
		(at start (robot-position ?wp ))
		
		;; point already acquired
		(at start (can-give-hint ?wp ))
		(at start (acquired-hint ?wp ))
		
		;; manipulator status
		(at start (manipulator-on ))
	)
	
	:effect (and
		;; robot status
		(at start (not (acquired )))
		(at end (thinking ))
		
		;; manipulator status
		(at start (not (manipulator-on )))
		(at end (manipulator-off ))
	)
)

;; === HINT ACQUISITION === ;;

(:durative-action signal-info-acquisition
	:parameters ( ?wp - waypoint )
	
	:duration (= ?duration 1)
	
	:condition (and
		;; robot status
		(at start (investigating ))
		
		;; manipulator status
		(at start (manipulator-off ))
		
		;; robot position
		(at start (robot-position ?wp ))
		
		;; moves
		(at start (> (remaining-moves ) 0))
	)
	
	:effect (and
		;; moves
		(at end (decrease (remaining-moves ) 1))
		
		;; robot status
		(at start (not (investigating )))
		(at end (ready-to-acquire ))
	)
)

(:durative-action acquire-hint
	:parameters ( ?wp - waypoint )
	
	:duration (= ?duration 1)
	
	:condition (and
		;; position
		(at start (robot-position ?wp ))
		
		;; robot state
		(at start (acquiring ))
		
		;; manipulator state
		(at start (manipulator-on ))
		
		;; it must be a hint there
		(at start (can-give-hint ?wp ))
		(at start (not-acquired-hint ?wp ))
	)
	
	:effect (and
		;; robot state
		(at end (not (acquiring )))
		(at end (acquired ))
		
		;; marker has been acquired
		(at end (not (not-acquired-hint ?wp )))
		(at end (acquired-hint ?wp ))
	)
)

(:durative-action sherlock-is-thinking
	:parameters ( ?wp - waypoint )
	
	:duration (= ?duration 1)
	
	:condition (and
		;; system status
		(at start (thinking ))
		
		;; manipulator status
		(at start (manipulator-off ))
		
		;; robot localization
		(at start (robot-position ?wp ))
		
		;; hint already acquired for that position
		(at start (can-give-hint ?wp ))
		(at start (acquired-hint ?wp ))
	)
	
	:effect (and
		;; system status
		(at start (not (thinking )))
		(at end (investigating ))
	)
)

;; === MYSTERY SOLUTION === ;;

(:durative-action bright-bulb-over-head
	:parameters ( ?wp - waypoint )
	
	:duration (= ?duration 1)
	
	:condition (and
		;; system status
		(at start (investigating ))
		
		;; robot position
		(at start (robot-position ?wp ))
		
		;; no more moves
		(at start (= (remaining-moves ) 0))
	)
	
	:effect (and
		(at start (not (investigating )))
		(at end (wanna-solve ))
	)
)

(:durative-action who-killed-doctor-black-huh-q1
	:parameters ( ?id - hypID )
	
	:duration (= ?duration 1)
	
	:condition (and
		;; system status
		(at start (who-killed-doctor-black ))
		
		;; robot position
		(at start (robot-position center))
		
		;; remaining moves
		(at start (= (remaining-moves ) 0))
		
		;; quality of the ID -- active
		(at start (h-open ?id ))
	)
	
	:effect (and
		;; reward - 1 point
		(at end (increase (solution-quality ) 1))
		
		;; status change
		(at start (not (who-killed-doctor-black )))
		(at end (elementary-whatson ))
	)
)

(:durative-action who-killed-doctor-black-huh-q2
	:parameters ( ?id - hypID )
	
	:duration (= ?duration 1)
	
	:condition (and
		;; system status
		(at start (who-killed-doctor-black ))
		
		;; robot position
		(at start (robot-position center))
		
		;; remaining moves
		(at start (= (remaining-moves ) 0))
		
		;; quality of the ID -- complete
		(at start (h-complete ?id ))
	)
	
	:effect (and
		;; reward - 10 points
		(at end (increase (solution-quality ) 10))
		
		;; status change
		(at start (not (who-killed-doctor-black )))
		(at end (elementary-whatson ))
	)
)

)
