
PKG DOCS -- robocluedo_mission_manager
==========================================

this is the center of the architecture for the robocluedo project.

The node provides three nodes:

- **mission_manager.cpp** : the main node of the architecture; the node which runs the investigation
- **manipulation_unit.cpp** in RCL#3 is used by the mission manager only in order to move the arm continuously; this movement makes the robot more capable of finding hints in the environment
- **navigation_unit.cpp** acts as a bridge between the ROSPlan actions requiring the navigation capabilities of the system, and the motion controllers of the system, guaranteeing the architectural separation between different components of the project.


.. toctree::
	:caption: User Documentation
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
