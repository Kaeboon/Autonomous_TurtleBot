cmake_minimum_required(VERSION 3.8)
project(ee4308_interfaces)

# find dependencies
find_package(ament_cmake REQUIRED)
find_package(std_msgs REQUIRED)
find_package(sensor_msgs REQUIRED)
find_package(geometry_msgs REQUIRED)
find_package(nav_msgs REQUIRED)
find_package(action_msgs REQUIRED)
find_package(rosidl_default_generators REQUIRED)
rosidl_generate_interfaces(${PROJECT_NAME}
  "srv/Waypoint.srv"
  DEPENDENCIES std_msgs sensor_msgs geometry_msgs nav_msgs action_msgs
)

install(
  DIRECTORY srv
  DESTINATION share/${PROJECT_NAME}/
)

ament_export_dependencies(rosidl_default_runtime)

ament_package()
