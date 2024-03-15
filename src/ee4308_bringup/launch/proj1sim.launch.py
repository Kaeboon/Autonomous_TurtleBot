import os

from ament_index_python.packages import get_package_share_directory
from launch import LaunchDescription
from launch.actions import IncludeLaunchDescription, DeclareLaunchArgument, OpaqueFunction, LogInfo
from launch.launch_description_sources import PythonLaunchDescriptionSource
from launch.substitutions import LaunchConfiguration
from launch_ros.actions import Node
import subprocess

def generate_launch_description():
    # should merge with labs with ee4308_task as argument.

    pkg_turtlebot3_gazebo = get_package_share_directory('turtlebot3_gazebo')
    pkg_gazebo_ros = get_package_share_directory('gazebo_ros')
    pkg_ee4308_turtle = get_package_share_directory('ee4308_turtle')
    pkg_ee4308_bringup = get_package_share_directory('ee4308_bringup')

    default_x_pose = '0.0' # to provide as YAML parameter in future
    default_y_pose = '0.0' # to provide as YAML parameter in future
    # default_world = os.path.join(pkg_turtlebot3_gazebo, 'worlds', 'turtlebot3_world.world')
    default_world = os.path.join(pkg_ee4308_bringup, 'worlds', 'ee4308_world_24.world')
    default_use_sim_time = 'true'
                                 
    # parameters
    x_pose = LaunchConfiguration('x_pose')
    x_pose_arg = DeclareLaunchArgument('x_pose', default_value=default_x_pose)
    
    y_pose = LaunchConfiguration('y_pose')
    y_pose_arg = DeclareLaunchArgument('y_pose', default_value=default_y_pose)
    
    world = LaunchConfiguration('world')
    world_arg = DeclareLaunchArgument('world', default_value=default_world)
    
    use_sim_time = LaunchConfiguration('use_sim_time')
    use_sim_time_arg = DeclareLaunchArgument('use_sim_time', default_value=default_use_sim_time)
    
    ee4308_task = "proj1"
    tbot_model = "burger"
    turtle_ns = "turtle" #note gazebo plugins require hacky namespace changes in sdf model files.

    # open gazebo
    gzserver_cmd = IncludeLaunchDescription(
        PythonLaunchDescriptionSource(
            os.path.join(pkg_gazebo_ros, 'launch', 'gzserver.launch.py')
        ),
        launch_arguments={'world': world}.items()
    )

    gzclient_cmd = IncludeLaunchDescription(
        PythonLaunchDescriptionSource(
            os.path.join(pkg_gazebo_ros, 'launch', 'gzclient.launch.py')
        )
    )

    # spawn robot state publisher
    urdf_path = os.path.join(pkg_turtlebot3_gazebo, 'urdf','turtlebot3_burger.urdf')
    with open(urdf_path, 'r') as info:
        robot_desc = info.read()
    start_robot_state_publisher = Node(
        package='robot_state_publisher',
        executable='robot_state_publisher',
        name='robot_state_publisher',
        output='screen',
        parameters=[{
            'use_sim_time': use_sim_time,
            'robot_description': robot_desc,
        }],
    )

    # spawn tbot in gazebo with namespacing
    model_path = os.path.join(pkg_ee4308_bringup, 'models', 'turtlebot3_burger', 'model.sdf')
    
    start_gazebo_ros_spawner_cmd = Node(
        namespace=turtle_ns,
        package='gazebo_ros',
        executable='spawn_entity.py',
        arguments=[
            '-entity', tbot_model,
            '-file', model_path,
            '-x', x_pose,
            '-y', y_pose,
            '-z', '0.01',
            "-robot_namespace", turtle_ns
        ],
        output='screen',
    )

    # rviz
    start_rviz = Node(
        package='rviz2',
        executable='rviz2',
        arguments=['-d', os.path.join(pkg_ee4308_bringup, "rviz", ee4308_task + ".rviz"),],
    )

    # publish a static transform because we are not so concerned about odom drift in this course.
    start_static_tf2_map_to_odom = Node(
        namespace=turtle_ns,
        package = "tf2_ros", 
        executable="static_transform_publisher",
        arguments = ["0", "0", "0", "0", "0", "0", "map", "/odom"] #turtle_ns + "/odom"]
    )

    # get params
    params_turtle = os.path.join(pkg_ee4308_bringup, 'params', ee4308_task + '.yaml')

    # start nodes
    start_turtle_estimator = Node(
        namespace=turtle_ns,
        package='ee4308_turtle',
        executable='estimator',
        parameters=[params_turtle],
        arguments=[x_pose, y_pose],
        output='screen',
    )

    start_turtle_mapper = Node(
        namespace=turtle_ns,
        package='ee4308_turtle',
        executable='mapper',
        parameters=[params_turtle],
        output='screen',
    )

    start_turtle_planner = Node(
        namespace=turtle_ns,
        package='ee4308_turtle',
        executable='planner_smoother',
        parameters=[params_turtle],
        output='screen',
    )
    
    start_turtle_behavior = Node(
        namespace=turtle_ns,
        package='ee4308_turtle',
        executable='behavior',
        parameters=[params_turtle],
        output='screen',
    )

    start_turtle_controller = Node(
        namespace=turtle_ns,
        package='ee4308_turtle',
        executable='controller',
        parameters=[params_turtle],
        output='screen',
    )

    # Create the launch description and populate
    ld = LaunchDescription()

    # Declare the launch options
    ld.add_action(x_pose_arg)
    ld.add_action(y_pose_arg)
    ld.add_action(world_arg)
    ld.add_action(use_sim_time_arg)

    ld.add_action(gzserver_cmd)
    ld.add_action(gzclient_cmd)
    ld.add_action(start_gazebo_ros_spawner_cmd)
    ld.add_action(start_robot_state_publisher)
    ld.add_action(start_rviz)

    ld.add_action(start_static_tf2_map_to_odom)

    ld.add_action(start_turtle_estimator)
    ld.add_action(start_turtle_mapper)
    ld.add_action(start_turtle_planner)
    ld.add_action(start_turtle_behavior)
    ld.add_action(start_turtle_controller)

    return ld