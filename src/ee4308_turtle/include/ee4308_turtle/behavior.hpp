#include <iostream>
#include <algorithm>
#include <cmath>
#include <vector>

#include <rclcpp/rclcpp.hpp>
#include <nav_msgs/msg/occupancy_grid.hpp>
#include <nav_msgs/msg/odometry.hpp>
#include <nav_msgs/srv/get_map.hpp>
#include <nav_msgs/srv/get_plan.hpp>
#include <geometry_msgs/msg/pose_stamped.hpp>

#include "ee4308_lib/core.hpp"
#include "ee4308_interfaces/srv/waypoint.hpp"

#pragma once
namespace ee4308::turtle
{
    struct BehaviorParameters
    { // contains defaults that can be overwritten
        struct Services
        {
            std::string goto_waypoint = "goto_waypoint";
        } services;
        std::vector<V2d> waypoints;
    };

    /**
     * The Behavior ROS Node that maintains subscribers and publishers for the Behavior class.
     */
    class ROSNodeBehavior : public rclcpp::Node
    {
    private:
        BehaviorParameters params_;
        rclcpp::Client<ee4308_interfaces::srv::Waypoint>::SharedPtr client_goto_waypoint_; // client

    public:
        /**
         * Constructor for the Behavior ROS Node.
         * @param name name of node.
         */
        ROSNodeBehavior(
            const std::string &name = "behavior")
            : Node(name)
        {
            // get all parameters from the param server.
            initParams();

            // Initialize services
            initServices();
        }

        /**
         * main run loop
         */
        void run()
        {
            for (const V2d &waypoint : params_.waypoints)
            {
                requestGotoWaypoint(waypoint);

                if (rclcpp::ok() == false)
                    break;
            }
        }

    private:
        /**
         * Initialize parameters from the parameter server
         */
        void initParams()
        {
            declare_parameter<std::string>("services.goto_waypoint", params_.services.goto_waypoint);
            get_parameter<std::string>("services.goto_waypoint", params_.services.goto_waypoint);
            std::cout << "services.goto_waypoint: " << params_.services.goto_waypoint << std::endl;

            // waypoints
            std::vector<double> flat;
            declare_parameter<std::vector<double>>("waypoints", flat);
            flat = get_parameter("waypoints").as_double_array();
            if (flat.size() > 2)
            {
                params_.waypoints.clear();
                for (size_t i = 1; i < flat.size(); i += 2) // ignore the last value if flat's size is odd numbered.
                    params_.waypoints.emplace_back(flat[i - 1], flat[i]);
            }
            std::cout << "waypoints: { ";
            for (const V2d &waypoint : params_.waypoints)
                std::cout << waypoint << "; ";
            std::cout << "}" << std::endl;
        }

        void initServices()
        {
            // Initialize services
            client_goto_waypoint_ = create_client<ee4308_interfaces::srv::Waypoint>(params_.services.goto_waypoint);

            // Wait for the service to respond.
            while (rclcpp::ok() == true &&
                   client_goto_waypoint_->wait_for_service(std::chrono::duration<int, std::milli>(200)) == false) // wait for 200ms
            {
                // RCLCPP_INFO_STREAM(get_logger(), "Waiting for service servers...");
            }
            RCLCPP_INFO_STREAM(get_logger(), "Finished services.");
        }

        /**
         * Requests a map from mapper. returns true if the map is received and the planner's internal cost map is updated.
         */
        bool requestGotoWaypoint(const V2d &waypoint)
        {
            std::cout << "Requesting waypoint ( " << waypoint << " )" << std::endl;

            // Send the request.
            auto request = std::make_shared<ee4308_interfaces::srv::Waypoint::Request>();
            request->waypoint.x = waypoint.x;
            request->waypoint.y = waypoint.y;
            auto result = client_goto_waypoint_->async_send_request(request);

            // block until complete or shutdown
            while (rclcpp::ok())
            {
                std::future_status status = result.future.wait_for(std::chrono::duration<int, std::milli>(200)); // wait for 200ms

                if (status == std::future_status::ready) // request succeeded
                    break;

                rclcpp::spin_some(get_node_base_interface()); // make sure the service response can reach this node.
            }

            // return false if shutdown
            if (rclcpp::ok() == false)
                return false; // Ctrl+C

            std::cout << "Waypoint ( " << waypoint << " ) reached." << std::endl;
            return true;
        }
    };
}