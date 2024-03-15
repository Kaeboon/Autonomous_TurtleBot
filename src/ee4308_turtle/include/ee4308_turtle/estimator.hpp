#include <iostream>
#include <algorithm>
#include <cmath>
#include <vector>

#include <rclcpp/rclcpp.hpp>
#include <nav_msgs/msg/odometry.hpp>
#include <sensor_msgs/msg/joint_state.hpp>
#include <sensor_msgs/msg/imu.hpp>

#include "ee4308_lib/core.hpp"

#pragma once
namespace ee4308::turtle
{
    struct EstimatorParameters
    { // contains defaults that can be overwritten
        struct Topics
        {
            std::string imu = "imu";
            std::string wheels = "joint_states";
            std::string pose = "pose";
        } topics;
        double imu_lin_gain = 0.5;
        double imu_ang_gain = 0.5;
        double frequency = 40;
        double axle_track = 0.16;
        double wheel_radius = 0.033;
        double straight_thres = 1e-9;
    };

    /**
     * The Estimator ROS Node that maintains subscribers and publishers for the Estimator class.
     */
    class ROSNodeEstimator : public rclcpp::Node
    {
    private:
        EstimatorParameters params_;
        rclcpp::Publisher<nav_msgs::msg::Odometry>::SharedPtr pub_pose_;
        rclcpp::Subscription<sensor_msgs::msg::Imu>::SharedPtr sub_imu_;
        rclcpp::Subscription<sensor_msgs::msg::JointState>::SharedPtr sub_wheels_;
        double imu_lin_acc, imu_ang_vel, wheel_left, wheel_right;

    public:
        /**
         * Constructor for the Estimator ROS Node.
         * @param name name of node.
         */
        ROSNodeEstimator(
            const std::string &name = "behavior")
            : Node(name)
        {
            // get all parameters from the param server.
            initParams();

            // Initialize services
            initTopics();
        }

        /**
         * main run loop
         */
        void run(V2d rbt_pos, double rbt_ang)
        {
            rclcpp::Rate rate(params_.frequency);
            rclcpp::Time last_estimate = now();

            double prev_wheel_left = wheel_left;   // when the hardware starts in proj 1, wheel_left can be non zero.
            double prev_wheel_right = wheel_right; // when the hardware starts in proj 1, wheel_right can be non zero.
            double lin_vel = 0, ang_vel = 0;
            double wheel_change_left, wheel_change_right, lin_vel_imu, lin_vel_wheel, ang_vel_imu, ang_vel_wheel;
            while (rclcpp::ok() == true)
            {
                // call all or some of the callbacks.
                rclcpp::spin_some(get_node_base_interface());

                // find the elapsed time
                double elapsed = (now() - last_estimate).seconds(); // the amount of time elapsed.
                if (elapsed < THRES)
                    continue; // ignore if the elapsed time is close to zero.
                last_estimate = now();

                // ===== FIXME =====
                wheel_change_left = wheel_left - prev_wheel_left;
                prev_wheel_left = wheel_left; // update wheel

                wheel_change_right = wheel_right - prev_wheel_right;
                prev_wheel_right = wheel_right;

                lin_vel_wheel = 2 * params_.wheel_radius * (wheel_change_right + wheel_change_left) / (4 * elapsed);
                lin_vel_imu = lin_vel + (elapsed * imu_lin_acc);
                lin_vel = params_.imu_lin_gain * lin_vel_imu + (1 - params_.imu_lin_gain) * lin_vel_wheel;

                ang_vel_wheel = 2 * params_.wheel_radius *  (wheel_change_right - wheel_change_left) / (2 * params_.axle_track * elapsed) ;
                ang_vel_imu = imu_ang_vel;
                ang_vel = params_.imu_ang_gain * ang_vel_imu + (1 - params_.imu_ang_gain) * ang_vel_wheel;


                if (abs(ang_vel) > abs(params_.straight_thres))
                {  
                    rbt_pos.x = rbt_pos.x + ((lin_vel / ang_vel) * (-sin(rbt_ang) + sin(rbt_ang + (ang_vel * elapsed))));
                    rbt_pos.y = rbt_pos.y + ((lin_vel / ang_vel) * (cos(rbt_ang) - cos(rbt_ang+ (ang_vel * elapsed))));
                    rbt_ang = rbt_ang + (ang_vel * elapsed);
                }
                else
                {
                    rbt_pos.x = rbt_pos.x + lin_vel * elapsed * cos(rbt_ang);
                    rbt_pos.y = rbt_pos.y + lin_vel * elapsed * sin(rbt_ang);
                    rbt_ang = rbt_ang + (ang_vel * elapsed);
                }

                // ===== end of FIXME =====

                // publish
                publishPose(rbt_pos, rbt_ang);

                // sleep for frequency
                rate.sleep();
            }
        }

    private:
        /**
         * Initialize parameters from the parameter server
         */
        void initParams()
        {
            declare_parameter<std::string>("topics.imu", params_.topics.imu);
            get_parameter<std::string>("topics.imu", params_.topics.imu);
            std::cout << "topics.imu: " << params_.topics.imu << std::endl;

            declare_parameter<std::string>("topics.wheels", params_.topics.wheels);
            get_parameter<std::string>("topics.wheels", params_.topics.wheels);
            std::cout << "topics.wheels: " << params_.topics.wheels << std::endl;

            declare_parameter<std::string>("topics.pose", params_.topics.pose);
            get_parameter<std::string>("topics.pose", params_.topics.pose);
            std::cout << "topics.pose: " << params_.topics.pose << std::endl;

            declare_parameter<double>("imu_lin_gain", params_.imu_lin_gain);
            get_parameter<double>("imu_lin_gain", params_.imu_lin_gain);
            std::cout << "imu_lin_gain: " << params_.imu_lin_gain << std::endl;

            declare_parameter<double>("imu_ang_gain", params_.imu_ang_gain);
            get_parameter<double>("imu_ang_gain", params_.imu_ang_gain);
            std::cout << "imu_ang_gain: " << params_.imu_ang_gain << std::endl;

            declare_parameter<double>("frequency", params_.frequency);
            get_parameter<double>("frequency", params_.frequency);
            std::cout << "frequency: " << params_.frequency << std::endl;

            declare_parameter<double>("wheel_radius", params_.wheel_radius);
            get_parameter<double>("wheel_radius", params_.wheel_radius);
            std::cout << "wheel_radius: " << params_.wheel_radius << std::endl;

            declare_parameter<double>("axle_track", params_.axle_track);
            get_parameter<double>("axle_track", params_.axle_track);
            std::cout << "axle_track: " << params_.axle_track << std::endl;

            declare_parameter<double>("straight_thres", params_.straight_thres);
            get_parameter<double>("straight_thres", params_.straight_thres);
            std::cout << "straight_thres: " << params_.straight_thres << std::endl;
        }

        /**
         * Initialize topics and wait until messages from all subscribed topics arrive.
         */
        void initTopics()
        {
            // Initialize publishers
            pub_pose_ = create_publisher<nav_msgs::msg::Odometry>(params_.topics.pose, 1);

            // Initialize messages with values that will never be written by their publishers.
            imu_lin_acc = NAN;
            wheel_left = NAN;

            // Initialize subscribers
            sub_wheels_ = create_subscription<sensor_msgs::msg::JointState>(
                params_.topics.wheels, 1, std::bind(&ROSNodeEstimator::subscriberCallbackWheels, this, std::placeholders::_1));
            sub_imu_ = create_subscription<sensor_msgs::msg::Imu>(
                params_.topics.imu, 1, std::bind(&ROSNodeEstimator::subscriberCallbackImu, this, std::placeholders::_1));

            // Wait for messages to arrive.
            rclcpp::Rate rate(5);
            while (rclcpp::ok() && (std::isnan(imu_lin_acc) || std::isnan(wheel_left)))
            {
                // RCLCPP_INFO_STREAM(get_logger(), "Waiting for topics...");
                rclcpp::spin_some(get_node_base_interface());
                rate.sleep();
            }
            RCLCPP_INFO_STREAM(get_logger(), "Finished waiting for topics...");
        }

        /**
         * The robot pose topic's subscriber callback
         */
        void subscriberCallbackWheels(const sensor_msgs::msg::JointState &msg)
        {
            wheel_left = msg.position[0];  // make sure msg.name[0] is left, and the wheel indeed change by the expected radians.
            wheel_right = msg.position[1]; // make sure msg.name[0] is right, and the wheel indeed change by the expected radians.
        }

        /**
         * Subscribe to IMU
         */
        void subscriberCallbackImu(const sensor_msgs::msg::Imu &msg)
        {
            imu_lin_acc = msg.linear_acceleration.x;
            imu_ang_vel = msg.angular_velocity.z;
        }

        /**
         * Subscribe to IMU
         */
        void publishPose(const V2d &rbt_pos, const double &rbt_ang)
        {
            // you can extend this to include velocities if you want, but the topic name may have to change from pose to something else.
            // odom is already taken.
            nav_msgs::msg::Odometry msg;
            msg.header.stamp = now();
            msg.child_frame_id = std::string(get_namespace()) + "/base_footprint";
            msg.header.frame_id = std::string(get_namespace()) + "/odom";
            msg.header.frame_id = "map";
            msg.pose.pose.position.x = rbt_pos.x;
            msg.pose.pose.position.y = rbt_pos.y;
            msg.pose.pose.orientation.w = cos(rbt_ang / 2);
            msg.pose.pose.orientation.x = 0;
            msg.pose.pose.orientation.y = 0;
            msg.pose.pose.orientation.z = sin(rbt_ang / 2);

            pub_pose_->publish(msg);
        }
    };
}
