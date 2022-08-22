
# HOW TO Run this Project

```{note}
If something would go south, please consider to run the project step by step; see the package robocluedo_mission_manager for further informations about how to run the project. 
```

## suggested way -- AMCL and no logs

The project is very heavy to run on a machine, reason for why I suggest to launch the project using these options:

- (two shells)
- use *amcl* for the localisation
- use Gazebo only, don't run RViz
- suppress every kind of output in the first shell using `1>/dev/null 2>/dev/null`

### Shell 1

all the components, without the `mission manager`

```bash
roslaunch robocluedo_mission_manager run_components.launch launch_mission_manager:=false robot_env_type:=gazebo robot_nav_type:=amcl 1>/dev/null 2>/dev/null
```

### Shell 2

the shell will show the output from the mission manager, to show hot the investigation is proceeding. 

```bash
roslaunch robocluedo_mission_manager run_components.launch init:=false launch_mission_manager:=true
```
