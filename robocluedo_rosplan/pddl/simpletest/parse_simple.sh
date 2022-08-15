#!/bin/bash

echo "===== running parser... ====="
../parser ./simpletest/simple_domain.pddl ./simpletest/simple_problem.pddl > parselog.log
echo "===== ...done ====="
