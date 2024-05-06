## Autonomous Bot

### Introduction

- The goal of this project is to move a robot around a flat area containing obstacles and narrow corridors. The robot must reach three waypoints, in order, and within 0.1i m (i = 1,2,3) of them, within 90s.

This program is developed using ROS2 Humble

### Running

<b> Running in simulation mode: </b>

1. Open `params.sh`, find and set `EE4308_TASK` to

```
export EE408_TASK=proj1sim
```

2. Build the program by running

```
. bd.sh
```

3. Run the program with

```
. run.sh
```

<b> Running in hardware mode: </b>

1. SSH from remote PC to turtlebot

```
ssh ubuntu@<TURTLEIP>
```

2. Turtle Bringup. Prepare the robot in a stationary position, and bring up the robot's sensors and peripherals using:

```
. robot.sh
```

wait for 5s to 10s, you will see two `Run!` on the output when completed.

3. Open `params.sh` in. Find and set the `EE4308_TASK` to

```
expore EE4308_TASK=proj1sim
```

4. Build your code

```
. bd.sh
```

5. Run your code

```
. run.sh
```

### Summary

<b> Task: Estimation Task</b>:

The program uses Odometry Motion Model and fuse it with the Inertial Measurement Unit (IMU) via weighted average.

<b> Task: Planner Task </b>:

The program utilizes A\* Algorithm to obtain the path towards the next waypoint.

<b> Task: Smoother Task </b>:

The program utilizes cubic hermite splines to smoothens the path.

<b> Task: Controller Task </b>:

The program implemented a pure pursuit path tracking algorithm. From the path found by the global planner, a lookahead point ahead of the closest point along the path is found. The robot subsequently moves in a circular trajectory towards the lookahead point.
