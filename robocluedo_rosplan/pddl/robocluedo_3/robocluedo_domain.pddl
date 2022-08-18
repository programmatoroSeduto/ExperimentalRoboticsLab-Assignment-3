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
	;; navigation system
	waypoint
)

(:predicates
	;; === LANDMARK replan === ;;
	(dirty )
	(not-dirty )
	
	;; === environment === ;;
	(is-center ?wp - waypoint )
	(not-is-center ?wp - waypoint )
	(at ?wp - waypoint )
	;; (not-at ?wp - waypoint )
	(passage ?wp1 ?wp2 - waypoint )
	(explored ?wp - waypoint )
	(not-explored ?wp - waypoint )
	(at-center )
	(not-at-center )
	
	;; === LANDMARK collect === ;;
	(hint-ready )
	(not-hint-ready )
	(hint-collected ?wp - waypoint )
	(not-hint-collected ?wp - waypoint )
)

(:functions
	
)

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

(:durative-action collect-hint
	:parameters ( ?wp - waypoint )
	
	:duration (= ?duration 0.99 )
	
	:condition (and
		;; status
		(at start (not-dirty ))
		(at start (hint-ready ))
		
		;; localization
		(at start (at ?wp ))
		(at start (not-is-center ?wp ))
		
		;; hint inside
		(at start (not-hint-collected ?wp ))
	)
	
	:effect (and
		;; status
		(at start (not (hint-ready )))
		(at end (not-hint-ready ))
		
		;; collected that hint
		(at start (not (not-hint-collected ?wp )))
		(at end (hint-collected ?wp ))
	)
)

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

)
