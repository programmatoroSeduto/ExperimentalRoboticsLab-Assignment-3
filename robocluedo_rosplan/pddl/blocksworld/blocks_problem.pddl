(define (problem blocks3)

	(:domain blocksworld)
	
	(:objects a b c)
	
	; tower of blocks: a,b,c
	(:init (on-table a)
			(on b a)
			(on c b)
			(clear c)
			(arm-empty))
	
	; tower of blocks: c,b,a        
	(:goal (and (on-table c)
				(on b c)
				(on a b)
				(clear a)
				(arm-empty)))

)
