
# HOW TO perform localization with AMCL

---

```{toctree}
---
caption: Contents
---
./mapping.md
```

---

```{note}
the package `worlds` already contains the map file created during this HOW TO.
```

## Save the map

the exploration performed with GMapping can be saved using `map_server map_saver`.

```bash
# enable the robot environment
roslaunch robocluedo_robot_hunter run2.launch sim_type:=gazeborviz 1>/dev/null 2>/dev/null

# do some exloration
rosrun teleop_keyboard teleop_twist_keyboard.py

# save the map
rosrun map_server map_saver -f /root/ros_ws/src/erl3/robocluedo_dependencies/worlds/data/assignment3_map
```

the output from the map_saver should be like this:

```text
root@3b17871017fd:~/ros_ws/src/erl3/_docs# rosrun map_server map_saver -f /root/ros_ws/src/erl3/robocluedo_movement_controller/data/map
[ INFO] [1661074988.866398200]: Waiting for the map
[ INFO] [1661074989.080666300, 1172.635000000]: Received a 1984 X 1984 map @ 0.050 m/pix
[ INFO] [1661074989.080824600, 1172.636000000]: Writing map occupancy data to /root/ros_ws/src/erl3/robocluedo_movement_controller/data/map.pgm
[ INFO] [1661074989.166739700, 1172.691000000]: Writing map occupancy data to /root/ros_ws/src/erl3/robocluedo_movement_controller/data/map.yaml
[ INFO] [1661074989.166996800, 1172.691000000]: Done
```

## Load the map with AMCL

here's how to run AMCL via launch file:

```
<?xml version="1.0"?>

<launch>

<node pkg="map_server" name="map_server" type="map_server" arg="$(find worlds)/data/assignment3_map.yaml" />

<param name="use_sim_time" value="true" />
<node pkg="amcl" type="amcl" name="amcl" output="screen" />
	<param name="base_frame_id" value="base_link" />
	<param name="min_particles" value="1000" />
	<param name="max_particles" value="5000" />
	<param name="odom_alpha1" value="2" />
	<param name="odom_alpha2" value="2" />
</node>

</launch>
```

here's the command to try the AMCL map of the environment:

```bash
roslaunch robocluedo_robot_hunter run2.launch sim_type:=gazeborviz launch_nav_stack:=false launch_amcl:=true 1>/dev/null 2>/dev/null

```
