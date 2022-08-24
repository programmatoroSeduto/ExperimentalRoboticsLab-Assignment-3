
.. _pkg-robocluedo-movement-controller:

PKG DOCS -- robocluedo_movement_controller
==========================================

.. note::
	this version of the movement controller has a lot of differences with the previous version:
	
	- integration with MoveBase and the navigation stack
	- improved launch files for running the project

this package enables the robot to move the manipulator and to navigate into the environment, usign different methods. 

here's a brief list of the nodes inside the package: 

- **bug_m.py** (composite behaviour) : the node performs a very simplified navigation point to point combining the two elementary behaviours *go_to_point* and *head_orientation*. 
	
	The node also implements strategie to gain manouver space before pointing the head in the direction of the target. 

- **nav_move_base.py** (composite behaviour) : another node performing the navigation point to point, using  *move_base* and the navigation stack with the elementary behaviour *head_orientation* for the final orientation.
	
	.. note::
		(RCL#3) the node is compatible with RoboCLuedo "hunter"

- **go_to_point_service_m.py** (elementary behaviour) : the node aligns the head in the direction of the target and moves along a straight line to the objective position
	
	the node is used to implement a very simplified navigation algorithm with no obstacles between the current position and the target one. It is switchable. 

- **head_orientation.py** (elementary behaviour) : orient the head with a given angle wrt the world frame
	
	the robot keeps turning around its vertical axis until the orientation isn't equal to the target one.

- **navigation_manager.cpp** : a high-level navigation service based on the concept of the controllers
	
	the node has two main objectives:
	
	- to provide a handy *abstraction* between the real implementation of tbe navigation system ...
	- ... and the service which is simply interested in reaching a (x,y,th) target
	
	.. note::
		(*RCL#3*) the project implements two controllers: ID=0 for nav_bug_m, and ID=1 for nav_move_base

- **manipulation_controller.cpp** : a (too much) simple implementation of a moveit-based manipulation controller capable of moving te arm to a given *preset pose*.
	
	.. note::
		(RCL#3) the node is compatible with RoboCLuedo "hunter"


.. toctree::
	:caption: User Documentation
	:titlesonly:
	:glob:
	
	./_docs/*


.. toctree::
	:caption: UML diagrams
	:glob:
	
	./_uml/*


.. toctree::
	:caption: code reference
	:glob:
	
	./_code_reference/*


.. toctree::
	:caption: Package Code Documentation
	:glob:
	
	./src/*
	./scripts/*


.. toctree::
	:caption: Code Examples
	:glob:
	
	./_examples/*
