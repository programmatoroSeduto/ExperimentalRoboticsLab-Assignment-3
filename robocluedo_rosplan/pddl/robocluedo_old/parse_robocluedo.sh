#!/bin/bash

echo "===== running parser... ====="
../parser ./robocluedo/robocluedo_domain.pddl ./robocluedo/robocluedo.pddl > parselog.log
echo "===== ...done ====="
