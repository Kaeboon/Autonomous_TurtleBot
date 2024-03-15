#include <memory>

#include <rclcpp/rclcpp.hpp>
#include <ee4308_turtle/controller.hpp>

int main(int argc, char *argv[])
{
    rclcpp::init(argc, argv);

    // controller node
    auto node = std::make_shared<ee4308::turtle::ROSNodeController>(
        "controller" // node name
    );

    rclcpp::executors::MultiThreadedExecutor exec;
    exec.add_node(node);
    exec.spin();

    rclcpp::shutdown();
    return 0;
}
