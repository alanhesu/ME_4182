#!/bin/bash
# apt-get update
source ~/.bashrc
source /opt/ros/kinetic

echo "Installing ROS packages"
apt-get install ros-kinetic-laser-filters -y
apt-get install ros-kinetic-robot-localization -y
apt-get install ros-kinetic-rplidar-ros -y
apt-get install ros-kinetic-razor-imu-9dof -y
apt-get install ros-kinetic-rosserial -y

echo "Remake catkin_ws"
cd ~/catkin_ws
catkin_make

echo "Make Arduino libraries"
cd ~/Arduino/libraries
rm -rf ros_lib
rosrun rosserial_arduino make_libraries.py .