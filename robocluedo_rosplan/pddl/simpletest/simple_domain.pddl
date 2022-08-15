
(define (domain dom)

(:requirements 
	:strips 
	:typing 
	:equality 
	:universal-preconditions 
	:fluents
	:durative-actions
)

(:types 
	boolobj
	key
)

(:predicates
	(b-not-true ?b - boolobj)
	(b-true ?b - boolobj)
	(start ) (stop ) (signal-stop )
)

(:functions
	(f-non-zero)
	(f-zero)
	(f-two-args ?k1 ?k2 - key)
)

(:durative-action set-start
	:parameters ( )

	:duration (= ?duration 1)

	:condition (at start (stop ))
	
	:effect (and
		(at end (not (stop )))
		(at end (start ))
	)
)

(:durative-action make-true
	:parameters (?b - boolobj)

	:duration (= ?duration 1)

	:condition (and
		(at start (b-not-true ?b))
		(at start (start))
	)
	
	:effect (and
		(at end (not (b-not-true ?b)))
		(at end (b-true ?b))
	)
)

(:durative-action make-false
	:parameters ( ?b - boolobj )

	:duration (= ?duration 1)

	:condition (and
		(at start (b-true ?b))
		(at start (start))
	)
	
	:effect (and
		(at end (not (b-true ?b)))
		(at end (b-not-true ?b))
	)
)

(:durative-action set-signal-stop
	:parameters ( )
	
	:duration (= ?duration 1)
	
	:condition (at start (start ))
	
	:effect (and
		(at end (signal-stop ))
	)
)

(:durative-action set-stop
	:parameters ( )

	:duration (= ?duration 1)

	:condition (and 
		(at start (start ))
		(at start (signal-stop ))
	)
	
	:effect (and
		(at end (not (start )))
		(at end (stop ))
		(at end (not (signal-stop )))
	)
)

)
