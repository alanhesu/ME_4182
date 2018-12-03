# ME_4182
Autonomous Golf Cart

# Installation instructions
Operating system requirement: Ubuntu 16.04 (Xenial)

Please make sure you have ROS Kinetic installed and your catkin workspace is configured.

If not, follow these tutorials to:

- Install ROS (http://wiki.ros.org/kinetic/Installation/Ubuntu)

- Configure your workspace (http://wiki.ros.org/ROS/Tutorials/InstallingandConfiguringROSEnvironment)



Additionally, install the Arduino IDE and place the sketchbook folder in your home directory (This should happen by default).



To clone this, run:
```
cd ~/catkin_ws/src
git clone https://github.com/alanhesu/ME_4182.git
```

And to install all necessary packages and libraries, run:
```
cd ME_4182
sudo ./package_install.sh
```

# Usage
Upload the `arduino_control.ino` code to the onboad Arduino Mega, which can be found in `arduino_control`

A configuration file is used to plan the path for the cart and tune its movement. This can be found in `drive_by_wire/params/drive_by_wire.yaml`

Ensure that all USB ports are properly assigned to their respective serial ports by changing the port values inside `drive_by_wire/launch/drive_by_wire.launch`

Finally, to startup and run the planned sequence, run:
```
roslaunch drive_by_wire drive_by_wire.launch
```
