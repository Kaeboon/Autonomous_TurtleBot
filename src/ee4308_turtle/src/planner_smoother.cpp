#include <memory>

#include <rclcpp/rclcpp.hpp>
#include <ee4308_turtle/planner_smoother.hpp>

int main(int argc, char *argv[])
{
    rclcpp::init(argc, argv);

    // planner node
    auto node = std::make_shared<ee4308::turtle::ROSNodePlannerSmoother>(
        "planner_smoother"            // node name
    );

    rclcpp::executors::MultiThreadedExecutor exec;
    exec.add_node(node);
    exec.spin();

    rclcpp::shutdown();
    return 0;
}
