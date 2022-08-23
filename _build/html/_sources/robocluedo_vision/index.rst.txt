
.. _pkg-robocluedo-vision:

PKG DOCS -- robocluedo_vision
==========================================

This package is needed in order to read the ArUco markers across the 
evironment. The detection has been designed to be as much quick as possible. 

The package contains only two nodes:

- **aruco_detect.cpp** implements the detection of the ArUco markers from the four camera streams. It is implemented in order to give a good performances. The node publishes the IDs found in the environment durig the exploration; *these IDs shall be decoded*. 
- **aruco_decode.py** decodes the ID, i.e. for each ID received from the node *aruco_detect*, the node asks the Oracle to returnt the hint associated to that ID, and finally publishes the hint. 


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
