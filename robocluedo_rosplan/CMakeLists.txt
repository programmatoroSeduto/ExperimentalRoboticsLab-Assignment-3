cmake_minimum_required(VERSION 3.0.2)
project(robocluedo_rosplan)

## Compile as C++11, supported in ROS Kinetic and newer
# add_compile_options(-std=c++11)

## Find catkin macros and libraries
## if COMPONENTS list like find_package(catkin REQUIRED COMPONENTS xyz)
## is used, also find other catkin packages
find_package(catkin REQUIRED COMPONENTS
  diagnostic_msgs
  robocluedo_rosplan_msgs
  roscpp
  rosplan_dependencies
  rosplan_dispatch_msgs
  rosplan_knowledge_msgs
  rosplan_planning_system
  rospy
  std_msgs
  std_srvs
)

## System dependencies are found with CMake's conventions
# find_package(Boost REQUIRED COMPONENTS system)


## Uncomment this if the package has a setup.py. This macro ensures
## modules and global scripts declared therein get installed
## See http://ros.org/doc/api/catkin/html/user_guide/setup_dot_py.html
# catkin_python_setup()

################################################
## Declare ROS messages, services and actions ##
################################################

## To declare and build messages, services or actions from within this
## package, follow these steps:
## * Let MSG_DEP_SET be the set of packages whose message types you use in
##   your messages/services/actions (e.g. std_msgs, actionlib_msgs, ...).
## * In the file package.xml:
##   * add a build_depend tag for "message_generation"
##   * add a build_depend and a exec_depend tag for each package in MSG_DEP_SET
##   * If MSG_DEP_SET isn't empty the following dependency has been pulled in
##     but can be declared for certainty nonetheless:
##     * add a exec_depend tag for "message_runtime"
## * In this file (CMakeLists.txt):
##   * add "message_generation" and every package in MSG_DEP_SET to
##     find_package(catkin REQUIRED COMPONENTS ...)
##   * add "message_runtime" and every package in MSG_DEP_SET to
##     catkin_package(CATKIN_DEPENDS ...)
##   * uncomment the add_*_files sections below as needed
##     and list every .msg/.srv/.action file to be processed
##   * uncomment the generate_messages entry below
##   * add every package in MSG_DEP_SET to generate_messages(DEPENDENCIES ...)

## Generate messages in the 'msg' folder
# add_message_files(
#   FILES
#   Message1.msg
#   Message2.msg
# )

## Generate services in the 'srv' folder
# add_service_files(
#   FILES
#   Service1.srv
#   Service2.srv
# )

## Generate actions in the 'action' folder
# add_action_files(
#   FILES
#   Action1.action
#   Action2.action
# )

## Generate added messages and services with any dependencies listed here
# generate_messages(
#   DEPENDENCIES
#   diagnostic_msgs#   robocluedo_rosplan_msgs#   rosplan_dispatch_msgs#   rosplan_knowledge_msgs#   std_msgs
# )

################################################
## Declare ROS dynamic reconfigure parameters ##
################################################

## To declare and build dynamic reconfigure parameters within this
## package, follow these steps:
## * In the file package.xml:
##   * add a build_depend and a exec_depend tag for "dynamic_reconfigure"
## * In this file (CMakeLists.txt):
##   * add "dynamic_reconfigure" to
##     find_package(catkin REQUIRED COMPONENTS ...)
##   * uncomment the "generate_dynamic_reconfigure_options" section below
##     and list every .cfg file to be processed

## Generate dynamic reconfigure parameters in the 'cfg' folder
# generate_dynamic_reconfigure_options(
#   cfg/DynReconf1.cfg
#   cfg/DynReconf2.cfg
# )

###################################
## catkin specific configuration ##
###################################
## The catkin_package macro generates cmake config files for your package
## Declare things to be passed to dependent projects
## INCLUDE_DIRS: uncomment this if your package contains header files
## LIBRARIES: libraries you create in this project that dependent projects also need
## CATKIN_DEPENDS: catkin_packages dependent projects also need
## DEPENDS: system dependencies of this project that dependent projects also need
# catkin_package(
#  INCLUDE_DIRS include
#  LIBRARIES robocluedo_rosplan
#  CATKIN_DEPENDS diagnostic_msgs robocluedo_rosplan_msgs roscpp rosplan_dependencies rosplan_dispatch_msgs rosplan_knowledge_msgs rospy std_msgs std_srvs
#  DEPENDS system_lib
# )
catkin_package(
 INCLUDE_DIRS include
 LIBRARIES 
	knowledge_base_tools 
	robocluedo_rosplan_actions
	# robocluedo_rosplan_action_interface
 CATKIN_DEPENDS 
	roscpp 
	std_msgs 
	std_srvs 
	diagnostic_msgs
	rosplan_dispatch_msgs 
	rosplan_knowledge_msgs 
	rosplan_planning_system
#  DEPENDS system_lib
)

###########
## Build ##
###########

## Specify additional locations of header files
## Your package locations should be listed before other locations
include_directories(
  include
  robocluedo_rosplan_actions
  ${catkin_INCLUDE_DIRS}
)

## Declare a C++ library
# add_library(${PROJECT_NAME}
#   src/${PROJECT_NAME}/robocluedo_rosplan.cpp
# )

add_library( kb_tools src/knowledge_base_tools/kb_tools.cpp )
target_link_libraries( kb_tools ${catkin_LIBRARIES} )

add_library( feedback_manager src/knowledge_base_tools/feedback_manager.cpp )
target_link_libraries( feedback_manager ${catkin_LIBRARIES} )

## Add cmake target dependencies of the library
## as an example, code may need to be generated before libraries
## either from message generation or dynamic reconfigure
# add_dependencies(${PROJECT_NAME} ${${PROJECT_NAME}_EXPORTED_TARGETS} ${catkin_EXPORTED_TARGETS})

## Declare a C++ executable
## With catkin_make all packages are built within a single CMake context
## The recommended prefix ensures that target names across packages don't collide
# add_executable(${PROJECT_NAME}_node src/robocluedo_rosplan_node.cpp)

add_executable( kb_interface src/kb_interface.cpp )
add_dependencies( kb_interface ${${PROJECT_NAME}_EXPORTED_TARGETS} ${catkin_EXPORTED_TARGETS} )
target_link_libraries( kb_interface ${catkin_LIBRARIES} kb_tools )

add_library( replan src/robocluedo_rosplan_actions/replan.cpp )
target_link_libraries( replan ${catkin_LIBRARIES} feedback_manager kb_tools rosplan_action_interface )
add_executable( node_replan src/robocluedo_rosplan_actions/replan_node.cpp )
add_dependencies( node_replan ${${PROJECT_NAME}_EXPORTED_TARGETS} ${catkin_EXPORTED_TARGETS} )
target_link_libraries( node_replan ${catkin_LIBRARIES} feedback_manager kb_tools rosplan_action_interface replan )

add_library( move_to src/robocluedo_rosplan_actions/move_to.cpp )
target_link_libraries( move_to ${catkin_LIBRARIES} feedback_manager kb_tools rosplan_action_interface )
add_executable( node_move_to src/robocluedo_rosplan_actions/move_to_node.cpp )
add_dependencies( node_move_to ${${PROJECT_NAME}_EXPORTED_TARGETS} ${catkin_EXPORTED_TARGETS} )
target_link_libraries( node_move_to ${catkin_LIBRARIES} feedback_manager kb_tools rosplan_action_interface move_to )

add_library( collect_hint src/robocluedo_rosplan_actions/collect_hint.cpp )
target_link_libraries( collect_hint ${catkin_LIBRARIES} feedback_manager kb_tools rosplan_action_interface )
add_executable( node_collect_hint src/robocluedo_rosplan_actions/collect_hint_node.cpp )
add_dependencies( node_collect_hint ${${PROJECT_NAME}_EXPORTED_TARGETS} ${catkin_EXPORTED_TARGETS} )
target_link_libraries( node_collect_hint ${catkin_LIBRARIES} feedback_manager kb_tools rosplan_action_interface collect_hint )

add_library( move_to_center src/robocluedo_rosplan_actions/move_to_center.cpp )
target_link_libraries( move_to_center ${catkin_LIBRARIES} feedback_manager kb_tools rosplan_action_interface )
add_executable( node_move_to_center src/robocluedo_rosplan_actions/move_to_center_node.cpp )
add_dependencies( node_move_to_center ${${PROJECT_NAME}_EXPORTED_TARGETS} ${catkin_EXPORTED_TARGETS} )
target_link_libraries( node_move_to_center ${catkin_LIBRARIES} feedback_manager kb_tools rosplan_action_interface move_to_center )

## Rename C++ executable without prefix
## The above recommended prefix causes long target names, the following renames the
## target back to the shorter version for ease of user use
## e.g. "rosrun someones_pkg node" instead of "rosrun someones_pkg someones_pkg_node"
# set_target_properties(${PROJECT_NAME}_node PROPERTIES OUTPUT_NAME node PREFIX "")

## Add cmake target dependencies of the executable
## same as for the library above
# add_dependencies(${PROJECT_NAME}_node ${${PROJECT_NAME}_EXPORTED_TARGETS} ${catkin_EXPORTED_TARGETS})

## Specify libraries to link a library or executable target against
# target_link_libraries(${PROJECT_NAME}_node
#   ${catkin_LIBRARIES}
# )

#############
## Install ##
#############

# all install targets should use catkin DESTINATION variables
# See http://ros.org/doc/api/catkin/html/adv_user_guide/variables.html

## Mark executable scripts (Python etc.) for installation
## in contrast to setup.py, you can choose the destination
catkin_install_python(PROGRAMS
  scripts/rosplan_pipeline_manager.py
#   scripts/rosplan_simulated_motion_system.py
  DESTINATION ${CATKIN_PACKAGE_BIN_DESTINATION}
)

## Mark executables for installation
## See http://docs.ros.org/melodic/api/catkin/html/howto/format1/building_executables.html
# install(TARGETS ${PROJECT_NAME}_node
#   RUNTIME DESTINATION ${CATKIN_PACKAGE_BIN_DESTINATION}
# )

## Mark libraries for installation
## See http://docs.ros.org/melodic/api/catkin/html/howto/format1/building_libraries.html
# install(TARGETS ${PROJECT_NAME}
#   ARCHIVE DESTINATION ${CATKIN_PACKAGE_LIB_DESTINATION}
#   LIBRARY DESTINATION ${CATKIN_PACKAGE_LIB_DESTINATION}
#   RUNTIME DESTINATION ${CATKIN_GLOBAL_BIN_DESTINATION}
# )

## Mark cpp header files for installation
# install(DIRECTORY include/${PROJECT_NAME}/
#   DESTINATION ${CATKIN_PACKAGE_INCLUDE_DESTINATION}
#   FILES_MATCHING PATTERN "*.h"
#   PATTERN ".svn" EXCLUDE
# )

## Mark other files for installation (e.g. launch and bag files, etc.)
# install(FILES
#   # myfile1
#   # myfile2
#   DESTINATION ${CATKIN_PACKAGE_SHARE_DESTINATION}
# )

#############
## Testing ##
#############

## Add gtest based cpp test target and link libraries
# catkin_add_gtest(${PROJECT_NAME}-test test/test_robocluedo_rosplan.cpp)
# if(TARGET ${PROJECT_NAME}-test)
#   target_link_libraries(${PROJECT_NAME}-test ${PROJECT_NAME})
# endif()

## Add folders to be run by python nosetests
# catkin_add_nosetests(test)
