
# HOW TO setup ArUco and OpenCV

```{note}
You don't need to download these packages: the project already includes these dependencies in the folder `robocluedo_dependencies`. 
```

---

```{toctree}
---
caption: Contents
---
./opencv-aruco-setup.md
```

---

## OpenCV for ROS

download in the workspace this:

```bash
git clone https://github.com/ros-perception/vision_opencv.git

```

### OpenCV Examples

download this package:

```bash
git clone https://github.com/CarmineD8/bridge_example.git

```

## ArUco

first of all, download this package:

```bash
git clone https://github.com/CarmineD8/aruco_ros.git -b main

```

the package contains the images to import as markers, in the folder `/aruco_ros/aruco_ros/models`. you should copy all the content of this folder inside your Gazebo environment `/root/.gazebo/models`. 

## Importing OpenCV and ArUco

here's the content to apply to your CMakeLists.txt file:


```cmake
find_package(OpenCV REQUIRED)

find_package(catkin REQUIRED COMPONENTS
  roscpp
# ...
	cv_bridge
	image_transport
  aruco
  aruco_msgs
  sensor_msgs
  dynamic_reconfigure
)

generate_dynamic_reconfigure_options(
  cfg/ArucoThreshold.cfg
)

add_compile_options(-std=c++11)
```

```cmake
###################################
## catkin specific configuration ##
###################################

catkin_package(
#  INCLUDE_DIRS include
#  LIBRARIES ...
#  CATKIN_DEPENDS ...
#  DEPENDS system_lib
  DEPENDS OpenCV
)
```
