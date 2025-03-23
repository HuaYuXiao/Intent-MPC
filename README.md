# Intent Prediction-Driven Model Predictive Control for UAV Planning and Navigation in Dynamic Environment

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

# step 4: add environment variables to your ~/.bashrc
echo 'source path/to/uav_simulator/gazeboSetup.bash' >> ~/.bashrc
```


## Run a Quick Demo
```
# change reference trajectory directory in: Intent-MPC/autnomous_flight/cfg/mpc_navigation/flight_base.yaml
predefined_goal_directory: "{PATH_TO}/Intent-MPC/autonomous_flight/cfg/mpc_navigation/ref_trajectory.txt"

# change prebuilt map directory in: Intent-MPC/autnomous_flight/cfg/mpc_navigation/mapping_param.yaml
prebuilt_map_directory: "{PATH_TO}/Intent-MPC/autonomous_flight/cfg/saved_map/test.pcd"
```


```
# start the uav simulator
roslaunch uav_simulator start.launch

# launch the intent MPC navigation 
roslaunch remote_control intent_mpc_demo.launch
```


## Citation and Reference
```

```
