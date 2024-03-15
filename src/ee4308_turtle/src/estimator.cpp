#include <memory>

#include <rclcpp/rclcpp.hpp>
#include <ee4308_turtle/estimator.hpp>

int main(int argc, char *argv[])
{
    rclcpp::init(argc, argv);

    // mapper node
    auto node = std::make_shared<ee4308::turtle::ROSNodeEstimator>(
        "estimator"         // node name
    );

    ee4308::V2d rbt_pos = {std::stod(argv[1]), std::stod(argv[2])};
    double rbt_ang = 0;

    node->run(rbt_pos, rbt_ang);

    rclcpp::shutdown();
}
