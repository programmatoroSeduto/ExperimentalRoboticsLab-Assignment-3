
.. _setup-the-project:

HOW TO setup the project
================================

.. toctree::
	:caption: Contents
	:maxdepth: 3
	
	./project-setup.md

This document describes how to perform the *installation of the project*.

Docker Image
-----------------------------

.. attention::
	**strongly recommended!** This docher image contains all you need to run the simulation. 

I strongly suggest you to run the project inside this Docker container. `Here <https://hub.docker.com/r/carms84/exproblab>`_ is the official site of the image.

Run the image using this instruction:

.. code-block::
	 
	docker run -it -p 6080:80 -p 5900:5900 -p 8888:8888 --name RCL_project carms84/exproblab

You can access the machine typing *http://127.0.0.1:6080/* on your console, or better using tools such as VNC.

.. attention::
	there could be some problems in running the container for the first time. 

It happens each time the container is created for the first time after its creation: xvnc11 gives some problems. To solve the situation, first run a bash inside the machine using the following command:

.. code-block::
	
	docker exec -it RCL_project /bin/bash

and copy and paste this sequence of commands:

.. code-block::
	
	rm /tmp/.X1-lock
	/usr/bin/Xvfb :1 -screen 0 1024x768x16 &
	export DISPLAY=":1"
	export HOME="/root"
	export USER="/root"
	/usr/bin/openbox &
	/usr/bin/lxpanel --profile LXDE &
	/usr/bin/pcmanfm --desktop --profile LXDE &
	x11vnc -display :1 -xkb -forever -shared -repeat &

The docker is a bit old inside. It should be updated before going on, using this sequence of instructions:

.. code-block::
	
	sudo apt-key adv --keyserver keyserver.ubuntu.com --recv-keys 4EB27DB2A3B88B8B
	curl -s https://raw.githubusercontent.com/ros/rosdistro/master/ros.asc | sudo apt-key add -
	sudo curl -sSL https://raw.githubusercontent.com/ros/rosdistro/master/ros.key -o /usr/share/keyrings/ros-archive-keyring.gpg
	sudo apt-get update -y
	sudo apt-get upgrade -y



Setup the workspace
-----------------------------

.. note::
	not needed if you already have Docker with the image suggested above. 

You need, first of all, to create a new workspace **which must be called ros_ws**: it is very important, otherwise the setup won't work. Another important observation to do here is that **the workspace must be located in your /root folder.

You can quickly create a new workspace by running this command inside the */root* folder:

To do this, 

.. code-block:: 
	
	cd /root/
	git clone https://github.com/programmatoroSeduto/docker-exproblab-backup.git -b main ros_ws

The download is quite large, so it could take some minutes depending on your network. 

After downloaded everything, try to compile the workspace:

.. code-block:: 
	
	cd /root/ros_ws/
	catkin_make

.. note::
	remember to source the workspace! Put the source insideyour *.bashrc* file. 




build the aRMOR ontology
--------------------------

The project requires aRMOR as one of its essential component. Use this instructions to compile it:

.. code-block::
	
	cd /root/ros_ws/src/armor/armor
	chmod +x ./gradlew
	./gradlew deployApp
	cd /root/ros_ws
	catkin_make
	




Install the MoveIt! framework
---------------------------------------

Use this simple bash script for installing MoveIt. The project requires the version 1.1.5 : 

.. code-block:: 
	
	cd /root/ros_ws/src
	sudo apt install -y ros-noetic-moveit
	apt-get remove -y ros-noetic-moveit*
	apt-get remove -y ros-noetic-srdfdom
	git clone https://github.com/ros-planning/moveit.git
	cd moveit
	git checkout 2b881e5e3c4fd900d4d4310f4b12f9c4a63eb5dd
	cd ..
	git clone https://github.com/ros-planning/moveit_resources.git
	cd moveit_resources
	git checkout f6a7d161e224b9909afaaf621822daddf61b6f52
	cd ..
	git clone https://github.com/ros-planning/srdfdom.git
	cd srdfdom
	git checkout b1d67a14e45133928f9793e9ee143998219760fd
	cd ..
	apt-get install -y ros-noetic-rosparam-shortcuts
	cd ..
	catkin_make
	catkin_make

this step requires a lot of time...

.. note::
	the fact that there is the same *catkin_make* twice at the end of the code is not a mistake: it could happen that the compiler crashes due to a lack of free memory. To continue the compilation, just call *catkin_make* again. 




(optional) Install the documentation system
--------------------------------------

.. note::
	Do it only if you're also interested in refreshing the documentation of the project.

This is the script to use for setting up Doxygen plus Sphinx, and some other components such as PlantUML.

.. code-block::
	
	# doxygen
	sudo apt-get install -y doxygen
	sudo apt-get install -y doxygen-gui
	
	# sphinx
	apt-get install python-setuptools python-dev build-essential  -y
	pip3 install sphinx==4.5.0
	pip3 install sphinxcontrib-fulltoc
	pip3 install rst2pdf
	pip3 install sphinxcontrib-needs
	pip3 uninstall docutils -y
	pip3 install docutils
	pip3 install sphinx-needs
	
	# breathe
	pip3 install breathe
	pip3 install sphinx-rtd-theme
	pip3 install myst-parser
	
	# openjdk 13
	sudo apt-get install -y openjdk-13-jre
	
	# plantUML
	sudo apt-get install -y graphviz
	pip3 install sphinxcontrib-plantuml




Download the repository
----------------------------------

It's time to download and install the project RCL#3 ! You can use this quick bash script for downloading and compiling the project:

.. code-block::
	
	cd /root/ros_ws/src
	git clone https://github.com/programmatoroSeduto/ExperimentalRoboticsLab-Assignment-3.git -b main ./erl3
	cd ..
	catkin_make




Install the simulation environment
--------------------------------------

The simulation environment requires the ArUco markers available in your Gazebo installation. You can quickly do that using this bash command:

.. code-block::
	
	cp -r /root/ros_ws/src/erl3/robocluedo_dependencies/worlds/models/* /root/.gazebo/models





Test out the project!
---------------------

The project has been compiled. You're ready to try it! See how to run the project for further informations about how to run the architecture. 

.. seealso::
	see also :ref:`run-the-project`
