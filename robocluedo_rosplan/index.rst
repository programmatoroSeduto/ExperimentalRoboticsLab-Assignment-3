
.. _pkg-robocluedo-rosplan:

PKG DOCS -- robocluedo_rosplan
==========================================

this package is in charge to "drive" ROSPlan in the way the project requires. In particular, it includes

- a PDDL mode of the problem the RoboCLuedo has to solve
- a PDDL/ROS implementation of the PDDL actions
- services to control the planning and the execution of the plan

it is designed as a *standalone module*: when a functionality is required, the system opens a service client and assumes that something else han implemented it. 

Here's a list of nodes inside this package, with a little of explaination about their functions.

Libraries
----------

- **knowledge_base_tools/feedback_manager.h** implements a simple feedback system used for providing further informations to the pipeline manager in case of failure in applying one ROSPlan action.

- **knowledge_base_tools/kb_interface.h** implements some basic service calls the nodes can be used to alter the ROSPlan knowledge base 

ROSPlan Actions
-----------------

- (UNUSED) **collect_node.cpp** implements the PDDL action ``(collect ?wp )`` which first moves the manipulator inside the marker, and then moves back it to its home position

- **move_to_node.cpp** and **move_to_center_node.cpp** are implementations respectively of the PDDL actions ``(move-to ?from ?to )`` and ``(move-to-center ?from )``, that make use of the navigation system to move the robot from one waypoint to another one. 

- **replan_node.cpp** implements ``(replan )`` whose the only purpose is to clear the knowledge base before starting. The action is called at the beginning of the plan if the robot is in state ``(dirty )`` (see :any:`pddl-model` for further informations about how the PDDL model works)

Nodes
-----------------

- **rosplan_pipeline_manager.py** is meant to be a sort of *main node* for the package: it provides services for managing all the steps of the ROSPlan planning approach, starting from the problem loading to the dispatch phase where the solution is put into practice. The node also provides functionalities for explaining any unexpected situation it could happen during the planning process, taking into account the particular behaviours of the planning system in use and using the feedback messages from the actions. 

- **kb_interface.cpp** implements a set of basic services to handle the state of the ROSPlan knowledge base, especially before starting a new problem instance. The node is made to hide the technical particularities of the PDDL model, trying to provide a handy, "standard" interface to the other nodes. 

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
