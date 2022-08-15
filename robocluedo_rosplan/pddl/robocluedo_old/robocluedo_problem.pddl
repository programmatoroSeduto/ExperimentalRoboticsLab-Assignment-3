(define (problem task)

(:domain robocluedo)

(:objects
	;; ontology 
	id1 id2 id3 id4 id5 id6 - hypID
	
	;; environment
	w1 w2 w3 w4 - waypoint
	
	;; hypothesis elements
	missscarlett colonelmustard mrswhite mrgreen mrspeacock profplum - who
	conservatory lounge kitchen library hall study bathroom diningRoom billiardRoom - where
	candlestick dagger leadpipe revolver rope spanner - what
)

(:init
	;; problem metadata
	(= (number-of-ids-in-the-problem ) 6)
		
	;; init state of the system
	(pre-init )
	
	;; moves
	(= (remaining-moves ) 3)
	
	;; quality of the solution
	(= (solution-quality ) 0)
	
	;; environment data
	(passage w1 w2 )
		(passage w1 w3 )
		(passage w1 w4 )
		(passage w1 center )
	(passage w2 w1 )
		(passage w2 w3 )
		(passage w2 w4 )
		(passage w2 center )
	(passage w3 w1 )
		(passage w3 w2 )
		(passage w3 w4 )
		(passage w3 center )
	(passage w4 w1 )
		(passage w4 w2 )
		(passage w4 w3 )
		(passage w4 center )
	(passage center w1 )
		(passage center w2 )
		(passage center w3 )
		(passage center w4 )
	
	;; navigation system
	(robot-position center )
	(not-explored w1 )
	(not-explored w2 )
	(not-explored w3 )
	(not-explored w4 )
	
	;; manipulator
	(manipulator-off )
	
	;; hints gathering system
	(can-give-hint w1 )
		(not-acquired-hint w1 )
	(can-give-hint w2 )
		(not-acquired-hint w2 )
	(can-give-hint w3 )
		(not-acquired-hint w3 )
	(can-give-hint w4 )
		(not-acquired-hint w1 )
	
	;; hypotheses classification
	(h-open id1 )
		(= (h-count-who id1 ) 0)
		(= (h-count-where id1 ) 0)
		(= (h-count-what id1 ) 0)
	(h-open id2 )
		(= (h-count-who id2 ) 0)
		(= (h-count-where id2 ) 0)
		(= (h-count-what id2 ) 0)
	(h-open id3 )
		(= (h-count-who id3 ) 0)
		(= (h-count-where id3 ) 0)
		(= (h-count-what id3 ) 0)
	(h-open id4 )
		(= (h-count-who id4 ) 0)
		(= (h-count-where id4 ) 0)
		(= (h-count-what id4 ) 0)
	(h-open id5 )
		(= (h-count-who id5 ) 0)
		(= (h-count-where id5 ) 0)
		(= (h-count-what id5 ) 0)
	(h-open id6 )
		(= (h-count-who id6 ) 0)
		(= (h-count-where id6 ) 0)
		(= (h-count-what id6 ) 0)
	
	;; hypothesis counting
	(= (h-count-open ) 6)
	(= (h-count-complete ) 0)
	(= (h-count-discard ) 0)
)

(:goal (elementary-whatson ))

)
