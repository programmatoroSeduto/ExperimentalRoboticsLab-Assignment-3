#!/bin/bash



echo "====="
echo "====="
echo "====="
echo "===== UPDATING THE DOCKER CONTAINER ====="
echo "===== see https://chrisjean.com/fix-apt-get-update-the-following-signatures-couldnt-be-verified-because-the-public-key-is-not-available/"
echo "===== see https://answers.ros.org/question/379190/apt-update-signatures-were-invalid-f42ed6fbab17c654/"
echo "====="
sudo apt-key adv --keyserver keyserver.ubuntu.com --recv-keys 4EB27DB2A3B88B8B
curl -s https://raw.githubusercontent.com/ros/rosdistro/master/ros.asc | sudo apt-key add -
sudo curl -sSL https://raw.githubusercontent.com/ros/rosdistro/master/ros.key -o /usr/share/keyrings/ros-archive-keyring.gpg
sudo apt-get update -y
sudo apt-get upgrade -y
sudo apt-get update -y
sudo apt-get upgrade -y



echo "====="
echo "====="
echo "====="
echo "===== INSTALLING MOVEIT LATEST FROM APT ====="
echo "====="

sudo apt install -y ros-noetic-moveit



echo "====="
echo "====="
echo "====="
echo "===== UNINSTALLING MOVEIT LATEST ====="
echo "====="

apt-get remove -y ros-noetic-moveit*
apt-get remove -y ros-noetic-srdfdom



echo "====="
echo "====="
echo "====="
echo "===== DOWNLOADING MOVEIT 1.1.5 ====="
echo "====="

git clone https://github.com/ros-planning/moveit.git
cd moveit
git checkout 2b881e5e3c4fd900d4d4310f4b12f9c4a63eb5dd
cd ..

echo "-- Install moveit_msgs 0.11.2"
git clone https://github.com/ros-planning/moveit_msgs.git
cd moveit_msgs
git checkout 612d7d5bb1047f65acd495a7e632da78a621545d
cd ..

echo "-- Install moveit_resources 0.8.0"
git clone https://github.com/ros-planning/moveit_resources.git
cd moveit_resources
git checkout f6a7d161e224b9909afaaf621822daddf61b6f52
cd ..

echo "-- Install srdfdom (21/03/2021)"
git clone https://github.com/ros-planning/srdfdom.git
cd srdfdom
git checkout b1d67a14e45133928f9793e9ee143998219760fd
cd ..

apt-get install -y ros-noetic-rosparam-shortcuts



echo "====="
echo "====="
echo "====="
echo "===== MAKING THE WORKSPACE ====="
echo "====="

cd ..
catkin_make

# repeat twice in case of ccplus cache fatal
catkin_make
