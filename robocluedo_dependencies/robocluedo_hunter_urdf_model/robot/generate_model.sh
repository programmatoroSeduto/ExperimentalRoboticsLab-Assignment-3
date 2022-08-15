#! /bin/bash

# sudo apt-get install -y evince

echo "====="
echo "====="
echo "===== CLEANING PREVIOUS MODEL ====="
echo "====="

rm -rf ./model
mkdir model

echo "====="
echo "====="
echo "===== GENERATING MODEL ====="
echo "====="

rosrun xacro xacro robocluedo_xacro.xacro -o ./model/robocluedo_urdf.urdf

echo "====="
echo "====="
echo "===== CHECKING MODEL VALIDITY ====="
echo "====="

check_urdf ./model/robocluedo_urdf.urdf

urdf_to_graphiz ./model/robocluedo_urdf.urdf
mv robocluedo_robot.gv ./model/robocluedo_robot.gv
mv robocluedo_robot.pdf ./model/robocluedo_robot.pdf

echo "====="
echo "====="
echo "===== DONE ====="
echo "====="

# ls ./model
# evince --fullscreen ./model/robocluedo_urdf.gv
