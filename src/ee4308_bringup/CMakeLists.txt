cmake_minimum_required(VERSION 3.8)
project(ee4308_bringup)

if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  add_compile_options(-Wall -Wextra -Wpedantic)
endif()

# find dependencies
find_package(ament_cmake REQUIRED)

install(
  DIRECTORY launch rviz params models worlds
  DESTINATION share/${PROJECT_NAME}/
)

ament_export_dependencies(gazebo_ros_pkgs)

ament_package()
