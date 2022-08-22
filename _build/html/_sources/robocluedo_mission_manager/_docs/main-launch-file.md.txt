

# HOW TO -- the main launch file

---

```{toctree}
---
maxdepth: 3
caption: Contents
---
./main-launch-file.md
```

---

This document goes through how to run the project using the main launch file. It can be very helpful in running each part of the architecture, with the possibility to control each component hiding the details of each package. 

## In a hurry? Just run that!

```{note}
Honestly, I don't recommend to use this directly. 
```

The launch file you should use is `run_components.launch`. By calling that in this way, the entire project will be run

- with Gazebo and RViz
- with SLAM/GMapping and move_base
- *with a bunch of useless messages on the screen*

```bash
roslaunch robocluedo_mission_manager run_components.launch 

```

## The INIT option -- Run component by component

what about if you want to run just one component? Well, the launch file provides the option `init:=false`. In particular, it's important to point out that the following does nothing: it just runs the ROS master. 

```bash
roslaunch robocluedo_mission_manager run_components.launch init:=false
```

### run aRMOR

Running the aRMOR package is very simple:

```bash
roslaunch robocluedo_mission_manager run_components.launch init:=false launch_armor:=true
```

### run the robot environment

```{note}
this 'run.launch' file exposes the most important parameters only for the project. If you need more options, consider to directly use the launch file `robocluedo_robot_hunter run2.launch`. For instance, maybe you want to run another world file which is located outside the `worlds` package. Another case is if you want to run the robot in RViz only: this launch file has some problem in this case. 
```

In order to run the robot, you can use these arguments:

- `robot_env_type` (default: *gazebo*) : the tools for the simulations to use
	
	values: 
	
	- gazebo
	- rviz
	- gazeborviz
	
- `robot_nav_type` (default: *gmapping*) : thetype of navigation stack to use
	
	values:
	
	- gmapping
	- amcl (for the world assignment3.world only!)
	
- `world_name` (default: *assignment3.world*) : the world file to execute

here are the bash commands:

```bash
# default launch for the environment
roslaunch robocluedo_mission_manager run_components.launch init:=false launch_robot:=true

# the complete command
roslaunch robocluedo_mission_manager run_components.launch init:=false launch_robot:=true robot_env_type:=gazebo robot_nav_type:=amcl world_name:=assignment3.world
```

### run rosplan

you can easily launch the AI of the robot using the option `launch_rosplan:=true`. 

```bash
roslaunch robocluedo_mission_manager run_components.launch init:=false launch_rosplan:=true
```

### run the movement controller

```bash
roslaunch robocluedo_mission_manager run_components.launch init:=false launch_motion_controllers:=true
```

### run the vision module

the vision module opens a new window shwing what the cameras are seeing. 

```bash
roslaunch robocluedo_mission_manager run_components.launch init:=false launch_aruco:=true
```

### run the Oracle

the option `launch_oracle:=true` launches two nodes: the real oracle for the third assignment, and the markers publisher. 

```bash
# simple version
roslaunch robocluedo_mission_manager run_components.launch init:=false launch_oracle:=true

# the complete command
roslaunch robocluedo_mission_manager run_components.launch init:=false launch_oracle:=true publish_markers:=true
```

### run the mission manager

for running the mission manager package, there are two main options:

- `launch_mission_manager_components:=true` : launch the units inside the mission manager of the node, without the main node
- `launch_mission_manager:=true` : launch the main node (not the components!)

```bash
# components only
roslaunch robocluedo_mission_manager run_components.launch init:=false launch_mission_manager_components:=true

# main node only
roslaunch robocluedo_mission_manager run_components.launch init:=false launch_mission_manager:=true

# the entire mission_manager package
roslaunch robocluedo_mission_manager run_components.launch init:=false launch_mission_manager_components:=true launch_mission_manager:=true

``` 

### combine the launch options

of course, the init option allows you to combine different options. For example, if you need to run both aRMOR and the robot environment in one call, use this:

```bash
roslaunch robocluedo_mission_manager run_components.launch init:=false launch_armor:=true launch_robot:=true robot_nav_type:=amcl
``` 

## HOW TO run the project

Here are the main commands for running this complex project. 

### step by step

```{note}
recommended for testing the project after the installation. In case of problems, remember that almost all the nodes in the project (especially the ones implemented in C++) have a so-called `DEVELOP_MODE` and other options to understand what's wrong using logs on a shell. 
```

In a hurry? And you want to run the project step by step? Well, this little bash script is for you:

```bash
# launch the robot environment
roslaunch robocluedo_mission_manager run_components.launch init:=false launch_robot:=true robot_env_type:=gazebo robot_nav_type:=amcl 1>/dev/null 2>/dev/null

# launch the oracle
roslaunch robocluedo_mission_manager run_components.launch init:=false launch_oracle:=true

# launch the motion controllers
roslaunch robocluedo_mission_manager run_components.launch init:=false launch_motion_controllers:=true 1>/dev/null 2>/dev/null

# launch the vision system
roslaunch robocluedo_mission_manager run_components.launch init:=false launch_aruco:=true

# launch aRMOR interface
roslaunch robocluedo_mission_manager run_components.launch init:=false launch_armor:=true

# launch the robocluedo ROSPlan package
roslaunch robocluedo_mission_manager run_components.launch init:=false launch_rosplan:=true

# launch the mission manager units
roslaunch robocluedo_mission_manager run_components.launch init:=false launch_mission_manager_components:=true

# launch the main node
roslaunch robocluedo_mission_manager run_components.launch init:=false launch_mission_manager:=true
``` 

### the fastest way to run the project

this command also contains the most important settings for running the project:

```bash
roslaunch robocluedo_mission_manager run_components.launch robot_env_type:=gazeborviz robot_nav_type:=gmapping
```

these options allows you to launch the roject using for instance amcl and gazebo only. 

### the suggested way

I strongly suggest to launch the project in this way:

- don't launch RViz: use Gazebo only
- split the launch in two shells:
	- the first one shows the output from the mission manager
	- the second one runs everything else
- *suppress the output from the first shell*
	- using the option `1>/dev/null 2>/dev/null`
- use AMCL for the localisation

**shell 1**: the components of the architecture

```bash
roslaunch robocluedo_mission_manager run_components.launch launch_mission_manager:=false robot_env_type:=gazebo robot_nav_type:=amcl 1>/dev/null 2>/dev/null
```

**shell 2**: the mission manager

```bash
roslaunch robocluedo_mission_manager run_components.launch init:=false launch_mission_manager:=true
```
