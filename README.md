# Intent-Prediction Driven Model Predictive Control for UAV Planning and Navigation in Dynamic Environment

## Installation Guide
The system requirements for this repository are as follows. Please ensure your system meets these requirements:
- Ubuntu 18.04/20.04 LTS
- ROS Melodic/Noetic

Please follow the instructions below to install this package.
```
# step1: install dependencies
sudo apt install ros-${ROS_DISTRO}-octomap* && sudo apt install ros-${ROS_DISTRO}-mavros* && sudo apt install ros-${ROS_DISTRO}-vision-msgs

# step 2: clone this repo to your workspace
cd ~/catkin_ws/src
git clone https://github.com/Zhefan-Xu/Intent-MPC.git

# step 3: follow the standard catkin_make procedure
cd ~/catkin_ws
catkin_make
```
```


## Run Demo
```
# start simulator
roslaunch uav_simulator start.launch # recommand to use the corridor env for your first trial

# open the Rviz visualization
roslaunch remote_control mpc_navigation_rviz.launch

# run the navigation program
roslaunch autonomous_flight mpc_navigation.launch



## Citation and Reference
```

```
