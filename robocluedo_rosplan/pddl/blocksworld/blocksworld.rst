.. _pddl-blocksworld:

PDDL Example -- Blocks World
==============================

This project uses this problem just as benchmarking. In particular, it is a simple problem allowing to test the action implementation patterns for this project, as well as to explore a bit the power of ROSplan with a problem not yet considered in the official tutorials. 

BlocksWorld -- Domain
-----------------------

.. code-block:: LISP
	
	(define (domain blocksworld)
		
		; no negative preconditions
		; no negative goals
		(:requirements :strips)

		(:predicates
			; We have just blocks so we do need predicates for "types"
			; clear : a block has nothing on top of it 
			; on-table : a block is on the table 
			; arm-empty : the gripper is not holding any block 
			; on(x,y) : block x is stacked on top of block y 
			(clear ?x)    
			(on-table ?x)
			(arm-empty)
			(holding ?x)
			(on ?x ?y)
		)

		; An action to pickup a block (from the table)
		(:action pickup
			:parameters (?x)
			:precondition (and (clear ?x) (on-table ?x) (arm-empty))
			:effect (and (holding ?x) 
						 (not (clear ?x)) (not (on-table ?x)) (not (arm-empty)))
		)

		; An action to put down a block (to the table)
		(:action putdown
			:parameters (?x)
			:precondition (and (holding ?x))
			:effect (and (clear ?x) (on-table ?x) (arm-empty)
						 (not (holding ?x)))
		)
		
		; An action to pickup a block x from another block y
		(:action unstack
			:parameters (?x ?y)
			:precondition (and (clear ?x) (on ?x ?y) (arm-empty))
			:effect (and (clear ?y) (holding ?x)
						  (not (clear ?x)) (not (on ?x ?y)) (not (arm-empty)))
		)
		
		; An action to stack a block x on top of another block y
		(:action stack
			:parameters (?x ?y)
			:precondition (and (clear ?y) (holding ?x))
			:effect (and (clear ?x) (on ?x ?y) (arm-empty)
						 (not (clear ?y)) (not (holding ?x)))
		)
	)

BlocksWorld -- Problem
-----------------------

.. code-block:: LISP
	
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


Problem Solution
------------------

expected solution, using `PDDL online editor <http://editor.planning.domains>`__ :

.. code-block:: LISP
	
	(unstack c b)
	(putdown c)
	(unstack b a)
	(stack b c)
	(pickup a)
	(stack a b)

