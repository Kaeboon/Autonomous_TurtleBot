EE4308_WS=$HOME/ee4308
# cd $EE4308_WS
# make sure you are in the folder of this .sh file before running it.

source /opt/ros/humble/setup.bash
export TURTLEBOT3_MODEL=burger
export ROS_DOMAIN_ID=30 #TURTLEBOT3
source /usr/share/gazebo/setup.sh
export EE4308_TASK=proj1sim
#export EE4308_TASK=proj1

# export GAZEBO_MODEL_PATH=$GAZEBO_MODEL_PATH:$EE4308_WS/src/ee4308_bringup/
# export GAZEBO_RESOURCE_PATH=$GAZEBO_RESOURCE_PATH:$EE4308_WS/src/ee4308_bringup/worlds
