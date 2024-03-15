cmake_minimum_required(VERSION 3.8)
project(ee4308_turtle)


if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  add_compile_options(-Wall -Wextra -Wpedantic)
endif()

# find dependencies
find_package(ament_cmake REQUIRED)
find_package(rclcpp REQUIRED)
find_package(rcutils REQUIRED)
find_package(std_msgs REQUIRED)
find_package(sensor_msgs REQUIRED)
find_package(geometry_msgs REQUIRED)
find_package(nav_msgs REQUIRED)
find_package(std_srvs REQUIRED)
find_package(tf2 REQUIRED)
find_package(tf2_ros REQUIRED)
find_package(eigen3_cmake_module REQUIRED)
find_package(Eigen3)
find_package(ee4308_lib)
find_package(ee4308_interfaces)

set (DEPENDENCIES
  "rclcpp"
  "rclcpp_action"
  "rclcpp_components"
  "rcutils"
  "std_msgs"
  "sensor_msgs"
  "geometry_msgs"
  "nav_msgs"
  "std_srvs"
  "tf2"
  "tf2_ros"
  "Eigen3"
  "ee4308_lib"
  "ee4308_interfaces"
)

######## MAPPER ########
add_executable(mapper src/mapper.cpp)
target_include_directories(mapper PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
  $<INSTALL_INTERFACE:include>)
ament_target_dependencies(mapper ${DEPENDENCIES})
target_compile_features(mapper PUBLIC c_std_99 cxx_std_17)  # Require C99 and C++17
install(TARGETS mapper DESTINATION lib/${PROJECT_NAME})

######## PLANNER AND SMOOTHER ########
add_executable(planner_smoother src/planner_smoother.cpp)
target_include_directories(planner_smoother PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
  $<INSTALL_INTERFACE:include>)
ament_target_dependencies(planner_smoother ${DEPENDENCIES})
target_compile_features(planner_smoother PUBLIC c_std_99 cxx_std_17)  # Require C99 and C++17
install(TARGETS planner_smoother DESTINATION lib/${PROJECT_NAME})

######## CONTROLLER ########
add_executable(controller src/controller.cpp)
target_include_directories(controller PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
  $<INSTALL_INTERFACE:include>)
ament_target_dependencies(controller ${DEPENDENCIES})
target_compile_features(controller PUBLIC c_std_99 cxx_std_17)  # Require C99 and C++17
install(TARGETS controller DESTINATION lib/${PROJECT_NAME})

######## BEHAVIOR ########
add_executable(behavior src/behavior.cpp)
target_include_directories(behavior PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
  $<INSTALL_INTERFACE:include>)
ament_target_dependencies(behavior ${DEPENDENCIES})
install(TARGETS behavior DESTINATION lib/${PROJECT_NAME})

######## ESTIMATOR ########
add_executable(estimator src/estimator.cpp)
target_include_directories(estimator PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
  $<INSTALL_INTERFACE:include>)
ament_target_dependencies(estimator ${DEPENDENCIES})
install(TARGETS estimator DESTINATION lib/${PROJECT_NAME})

# install(
#   TARGETS mapper planner_smoother behavior controller
#   )

# if(BUILD_TESTING)
#   find_package(ament_lint_auto REQUIRED)
#   # the following line skips the linter which checks for copyrights
#   # comment the line when a copyright and license is added to all source files
#   set(ament_cmake_copyright_FOUND TRUE)
#   # the following line skips cpplint (only works in a git repo)
#   # comment the line when this package is in a git repo and when
#   # a copyright and license is added to all source files
#   set(ament_cmake_cpplint_FOUND TRUE)
#   ament_lint_auto_find_test_dependencies()
# endif()


# install(
#   DIRECTORY models
#   DESTINATION share/${PROJECT_NAME}/
# )

ament_export_include_directories(include)
ament_export_dependencies(eigen3_cmake_module)
ament_export_dependencies(Eigen3)

ament_package()
