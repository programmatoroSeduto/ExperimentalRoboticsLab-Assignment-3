#! /bin/bash

# con -T l'algoritmo trova in tempo record la soluzione
timeout 10 ../popf -T ./robocluedo_domain.pddl ./robocluedo_problem.pddl
