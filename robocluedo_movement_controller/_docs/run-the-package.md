
# HOW TO run the package

```{attention}
before running the package, make sure to have a robotic environment to run. 
```

This package can be launched using the `run.launch` package, providing these ways to launch the project:

## Navigation Type


```{note}
the move_base launch option requires move_base running and ready to use
```

```bash
# bug_m only
roslaunch robocluedo_movement_controller.launch nav_type:=bugm

# move_base only
roslaunch robocluedo_movement_controller.launch nav_type:=navstack

# both the navigation systems
roslaunch robocluedo_movement_controller.launch nav_type:=all

```

## one component

```bash
# navigation only
roslaunch robocluedo_movement_controller.launch launch_nav_controller:=true launch_manip_controller:=false

# manipulation only
roslaunch robocluedo_movement_controller.launch launch_manip_controller:=true launch_nav_controller:=false

# both (default)
roslaunch robocluedo_movement_controller.launch 

```

### combining the options

of course you can combine the options. for instance,

```bash
roslaunch robocluedo_movement_controller.launch launch_nav_controller:=true launch_manip_controller:=false nav_type:=navstack
```
