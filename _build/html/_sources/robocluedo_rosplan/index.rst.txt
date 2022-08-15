
.. _pkg-robocluedo-rosplan:

PKG DOCS -- robocluedo_rosplan
==========================================

this package is in charge to "drive" ROSPlan in the way the project requires. In particular, it includes

- a PDDL mode of the problem the RoboCLuedo has to solve
- a PDDL/ROS implementation of the PDDL actions
- services to control the planning and the execution of the plan

it is designed as a *standalone module*: when a functionality is required, the system opens a service client and assumes that something else han implemented it. 

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
