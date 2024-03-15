#include <iostream>
#include <algorithm>
#include <cmath>
#include <vector>
#include <mutex>
#include <thread>

#include <rclcpp/rclcpp.hpp>
#include <rclcpp_action/rclcpp_action.hpp>
#include <nav_msgs/srv/get_plan.hpp>
#include <nav_msgs/msg/odometry.hpp>
#include <nav_msgs/msg/path.hpp>
#include <geometry_msgs/msg/pose_stamped.hpp>
#include <geometry_msgs/msg/point_stamped.hpp>
#include <geometry_msgs/msg/twist.hpp>

#include "ee4308_interfaces/srv/waypoint.hpp"
#include "ee4308_lib/core.hpp"
#include "ee4308_turtle/raytracer.hpp"
#include "ee4308_turtle/grid.hpp"

#pragma once
namespace ee4308::turtle
{
    struct ControllerParameters
    { // contains defaults that can be overwritten
        struct Services
        {
            std::string get_plan = "get_plan";           // the service name to request the plan
            std::string goto_waypoint = "goto_waypoint"; // the service name to respond to a waypoint objective.
        } services;
        struct Topics
        {
            std::string pose = "pose";       // the topic to subscribe to the robot's pose
            std::string cmd_vel = "cmd_vel"; // the topic to publish the twist commands
            std::string lookahead = "lookahead"; // the topic to publish the lookahead point
        } topics;
        double nearby = 0.3;       // the threshold in meters to determine if a waypoint is close enough.
        double lookahead = 0.3;    // the maximum lookahead distance
        double max_lin_vel = 0.22; // the maximum linear velocity (m/s)
        double max_lin_acc = 1;    // the maxmum linear acceleration (m/s/s)
        double max_ang_vel = 2.84; // the maximum angular velocity (rad/s)
        double max_ang_acc = 4;    // the maxmum angular acceleration (rad/s/s)
        double plan_frequency = 1; // the rate to request a path
        double frequency = 20;     // the rate to run the controller
        double lookahead_lin_vel = 0.1; // the default value for the lookahead linear velocity
    };

    /**
     * The Controller ROS Node that maintains subscribers and publishers for the Controller class.
     */
    class ROSNodeController : public rclcpp::Node
    {
    private:
        ControllerParameters params_;
        geometry_msgs::msg::Pose msg_pose_;                                             // subscribed message written by callback
        rclcpp::Subscription<nav_msgs::msg::Odometry>::SharedPtr sub_pose_;             // subscriber
        rclcpp::Publisher<geometry_msgs::msg::Twist>::SharedPtr pub_cmd_vel_;           // subscriber
        rclcpp::Publisher<geometry_msgs::msg::PointStamped>::SharedPtr pub_lookahead_;           // subscriber
        rclcpp::Client<nav_msgs::srv::GetPlan>::SharedPtr client_plan_;                 // client
        rclcpp::Service<ee4308_interfaces::srv::Waypoint>::SharedPtr service_waypoint_; // service
        rclcpp::CallbackGroup::SharedPtr cb_group_;                                     // to allow all callbacks to simultaneously occur in the executor. Requires node to be added to MultiThreaderExecutor
        std::vector<V2d> plan_;
        std::mutex mutex_pose_;

    public:
        /**
         * Constructor for the Controller ROS Node.
         * @param name name of node.
         */
        ROSNodeController(
            const std::string &name = "controller")
            : Node(name)
        {
            // Initialize call back group
            cb_group_ = create_callback_group(rclcpp::CallbackGroupType::Reentrant);

            // get all parameters from the param server.
            initParams();

            // Initialize topics
            initTopics();

            // Initialize services
            initServices();
        }

    private:
        /**
         * Initialize parameters from the parameter server
         */
        void initParams()
        {
            declare_parameter<std::string>("services.get_plan", params_.services.get_plan);       // declare frame_id as parameter, using default value in params_.frame_id.
            get_parameter<std::string>("services.get_plan", params_.services.get_plan);           // if frame_id is set in the param server, writes the data in the param server to params_.frame_id.
            RCLCPP_INFO_STREAM(get_logger(), "services.get_plan: " << params_.services.get_plan); // output the value of params_.frame_id.

            declare_parameter<std::string>("services.goto_waypoint", params_.services.goto_waypoint);
            get_parameter<std::string>("services.goto_waypoint", params_.services.goto_waypoint);
            RCLCPP_INFO_STREAM(get_logger(), "services.goto_waypoint: " << params_.services.goto_waypoint);

            declare_parameter<std::string>("topics.pose", params_.topics.pose);
            get_parameter<std::string>("topics.pose", params_.topics.pose);
            RCLCPP_INFO_STREAM(get_logger(), "topics.pose: " << params_.topics.pose);

            declare_parameter<std::string>("topics.cmd_vel", params_.topics.cmd_vel);
            get_parameter<std::string>("topics.cmd_vel", params_.topics.cmd_vel);
            RCLCPP_INFO_STREAM(get_logger(), "topics.cmd_vel: " << params_.topics.cmd_vel);

            declare_parameter<std::string>("topics.lookahead", params_.topics.lookahead);
            get_parameter<std::string>("topics.lookahead", params_.topics.lookahead);
            RCLCPP_INFO_STREAM(get_logger(), "topics.lookahead: " << params_.topics.lookahead);

            declare_parameter<double>("nearby", params_.nearby);
            get_parameter<double>("nearby", params_.nearby);
            RCLCPP_INFO_STREAM(get_logger(), "nearby: " << params_.nearby);

            declare_parameter<double>("lookahead", params_.lookahead);
            get_parameter<double>("lookahead", params_.lookahead);
            RCLCPP_INFO_STREAM(get_logger(), "lookahead: " << params_.lookahead);

            declare_parameter<double>("max_lin_vel", params_.max_lin_vel);
            get_parameter<double>("max_lin_vel", params_.max_lin_vel);
            RCLCPP_INFO_STREAM(get_logger(), "max_lin_vel: " << params_.max_lin_vel);

            declare_parameter<double>("max_lin_acc", params_.max_lin_acc);
            get_parameter<double>("max_lin_acc", params_.max_lin_acc);
            RCLCPP_INFO_STREAM(get_logger(), "max_lin_acc: " << params_.max_lin_acc);

            declare_parameter<double>("max_ang_vel", params_.max_ang_vel);
            get_parameter<double>("max_ang_vel", params_.max_ang_vel);
            RCLCPP_INFO_STREAM(get_logger(), "max_ang_vel: " << params_.max_ang_vel);

            declare_parameter<double>("max_ang_acc", params_.max_ang_acc);
            get_parameter<double>("max_ang_acc", params_.max_ang_acc);
            RCLCPP_INFO_STREAM(get_logger(), "max_ang_acc: " << params_.max_ang_acc);

            declare_parameter<double>("frequency", params_.frequency);
            get_parameter<double>("frequency", params_.frequency);
            RCLCPP_INFO_STREAM(get_logger(), "frequency: " << params_.frequency);

            declare_parameter<double>("plan_frequency", params_.plan_frequency);
            get_parameter<double>("plan_frequency", params_.plan_frequency);
            RCLCPP_INFO_STREAM(get_logger(), "plan_frequency: " << params_.plan_frequency);
        }

        /**
         * Initialize topics and wait until messages from all subscribed topics arrive.
         */
        void initTopics()
        {
            // Initialize publishers
            pub_cmd_vel_ = create_publisher<geometry_msgs::msg::Twist>(params_.topics.cmd_vel, 1);
            pub_lookahead_ = create_publisher<geometry_msgs::msg::PointStamped>(params_.topics.lookahead, 1);

            // Initialize messages with values that will never be written by their publishers.
            msg_pose_.position.z = NAN;

            // Initialize subscribers
            sub_pose_ = create_subscription<nav_msgs::msg::Odometry>(
                params_.topics.pose, 1, std::bind(&ROSNodeController::subscriberCallbackPose, this, std::placeholders::_1));

            // Wait for messages to arrive.
            rclcpp::Rate rate(5);
            while (rclcpp::ok() && (std::isnan(msg_pose_.position.z)))
            {
                // RCLCPP_INFO_STREAM(get_logger(), "Waiting for topics...");
                rclcpp::spin_some(get_node_base_interface());
                rate.sleep();
            }
            RCLCPP_INFO_STREAM(get_logger(), "Finished waiting for topics...");
        }

        /**
         * Initialize services and wait for them to respond
         */
        void initServices()
        {
            // Initialize the client service
            client_plan_ = create_client<nav_msgs::srv::GetPlan>(params_.services.get_plan,
                                                                 rmw_qos_profile_services_default, cb_group_);

            // Initialize the service
            service_waypoint_ = create_service<ee4308_interfaces::srv::Waypoint>(
                params_.services.goto_waypoint,
                std::bind(&ROSNodeController::serviceGotoWaypoint, this, std::placeholders::_1, std::placeholders::_2),
                rmw_qos_profile_services_default, cb_group_);

            RCLCPP_INFO_STREAM(get_logger(), "Finished services.");
        }

        /**
         * The robot pose topic's subscriber callback. Thread safe.
         */
        void subscriberCallbackPose(const nav_msgs::msg::Odometry &msg)
        {
            const std::lock_guard<std::mutex> lock(mutex_pose_);
            msg_pose_ = msg.pose.pose;
        }

        /**
         * Gets the robot pose. Thread safe.
         */
        geometry_msgs::msg::Pose pose()
        {
            const std::lock_guard<std::mutex> lock(mutex_pose_);
            return msg_pose_; // creates a copy
        }

        /**
         * Publishes the cmd_vel
         */
        void publishCmdVel(const double &lin_vel, const double &ang_vel)
        {
            geometry_msgs::msg::Twist msg_cmd_vel;
            msg_cmd_vel.linear.x = lin_vel;
            msg_cmd_vel.angular.z = ang_vel;
            pub_cmd_vel_->publish(msg_cmd_vel);
        }

        /**
         * Publishes the lookahead
         */
        void publishLookahead(const V2d &lookahead_point)
        {
            geometry_msgs::msg::PointStamped msg_lookahead;
            msg_lookahead.header.stamp = now();
            msg_lookahead.header.frame_id = "/map";
            msg_lookahead.point.x = lookahead_point.x;
            msg_lookahead.point.y = lookahead_point.y;
            msg_lookahead.point.z = 0.02;
            pub_lookahead_->publish(msg_lookahead);
        }

        /**
         * The service callback to move the robot to the waypoint
         */
        void serviceGotoWaypoint(const std::shared_ptr<ee4308_interfaces::srv::Waypoint::Request> request,
                                 std::shared_ptr<ee4308_interfaces::srv::Waypoint::Response> response)
        {
            const V2d waypoint = {request->waypoint.x, request->waypoint.y};
            std::cout << "Request to go to waypoint ( " << waypoint << " ) received." << std::endl;

            rclcpp::Duration plan_period(std::chrono::seconds(int64_t(1. / params_.plan_frequency)));
            rclcpp::Time last_plan_time = now();
            rclcpp::Rate rate(params_.frequency);

            geometry_msgs::msg::Pose _pose = pose(); // use this because it is thread safe. This service runs simultaneously as the subscriber callback
            DOF3 rbt_dof3 = convertPose(_pose);
            auto result = requestPlan(rbt_dof3.position, waypoint);
            bool plan_request_active = true;

            // ===== TODO: INITIALIZE VARIABLES
            rclcpp::Time last_move_time = now();
            double lin_vel = 0, ang_vel = 0, lin_acc = 0, ang_acc = 0;
            // end of TODO ====

            while (rclcpp::ok())
            {
                _pose = pose(); // use this because it is thread safe. This service runs simultaneously as the subscriber callback
                rbt_dof3 = convertPose(_pose);

                // exit the loop if waypoint is reached.
                if ((rbt_dof3.position - waypoint).norm() < params_.nearby)
                {
                    std::cout << "Waypoint ( " << waypoint << " ) reached." << std::endl;
                    break;
                }

                // write into new plan if a prior request has completed.
                if (plan_request_active == true && result.future.valid() == true)
                {                                                                                  // copy the plan
                    std::vector<geometry_msgs::msg::PoseStamped> poses = result.get()->plan.poses; // copy. avoid referencing when threading.
                    plan_.clear();
                    for (const geometry_msgs::msg::PoseStamped &pose : poses)
                        plan_.emplace_back(pose.pose.position.x, pose.pose.position.y); // copy to plan
                    plan_request_active = false;
                }

                // check if a new plan is required
                bool need_plan = (now() - last_plan_time > plan_period && plan_.size() > 1) || plan_.empty() == true; // the empty condition is redundancy

                // start a new plan request only if there are no active requests
                if (need_plan == true && plan_request_active == false)
                { 
                    plan_request_active = true;
                    last_plan_time = now();
                    result = requestPlan(rbt_dof3.position, waypoint);
                }

                // prune all points that are within lookahead distance away from the robot
                prunePlan(rbt_dof3.position); // make sure that plan contains at least one point.

                // move the robot // FIXME
                moveRobot(plan_.back(), rbt_dof3.position, rbt_dof3.orientation,
                          lin_vel, ang_vel, lin_acc, ang_acc, last_move_time);

                rate.sleep();
            }

            // safety
            publishCmdVel(0, 0);

            (void)response;
        }

        /**
         * Moves the robot by sending velocity commands to the cmd_vel topic.
         * all parameters that are referenced (lin_vel, ... last_move_time) are recorded from the previous call.
         * @param lookahead_point The lookahead point to go to.
         * @param rbt_pos The robot's coordinates (m)
         * @param rbt_ang The robot's heading (rad)
         * @param lin_vel The linear velocity command of the robot. Value is from previous time step. (m/s)
         * @param ang_vel The angular velocity command. Value is from previous time step. (rad/s)
         * @param lin_acc The measured linear acceleration of the robot. Value is from previous time step. (m/s/s)
         * @param ang_acc The measured angular acceleration of the robot. Value is from previous time step. (rad/s/s)
         */
        void moveRobot(const V2d lookahead_point, const V2d rbt_pos, const double rbt_ang, // read only 
                       double &lin_vel, double &ang_vel, double &lin_acc, double &ang_acc, // previous values 
                       rclcpp::Time &last_move_time) 
        { 
            // get the duration elapsed since last call 
            double elapsed = (now() - last_move_time).seconds(); // the elapsed time since the last call, in seconds. 
            last_move_time = now();                              // move forward the last move time. 
 
            // ==== FIXME: currently a simplified rotation shim controller. Convert to pure pursuit with speed and acc constraints. 
            (void)lin_acc; // use (void) to suppress unused warnings by the compiler 
            (void)ang_acc; // use (void) to suppress unused warnings by the compiler 
 
            V2d error_axes = lookahead_point - rbt_pos; 
            double error_ang = atan2(error_axes.y, error_axes.x) - rbt_ang; 
            error_ang = limit_angle(error_ang); // constrain to -pi and pi radians. 
            double error_y = (error_axes.y * cos(rbt_ang)) - (error_axes.x * sin(rbt_ang)); 
            double error_x = (error_axes.x * cos(rbt_ang)) + (error_axes.y * sin(rbt_ang)); 
            double error_lin = error_axes.norm(); 
            double curvature = (2 * error_y)/(error_lin*error_lin); 
 
            //std::cout<<"error_x: "<<error_x<<" error_y: "<<error_y<<" error_ang: "<<error_ang<<" error_lin: "<<error_lin<<std::endl;

            if(error_x < 0){ 
                double new_lin_vel = params_.lookahead_lin_vel * (-1); 
                double new_ang_vel = curvature * new_lin_vel; 
 
                double new_lin_acc = (new_lin_vel - lin_vel)/elapsed; 
                double new_ang_acc = (new_ang_vel - ang_vel)/elapsed; 
 
                if (abs(new_lin_acc) < params_.max_lin_acc) 
                    new_lin_acc = new_lin_acc; 
                else 
                    new_lin_acc = params_.max_lin_acc * sgn(new_lin_acc); 
                 
                if (abs(new_ang_acc) < params_.max_ang_acc) 
                    new_ang_acc = new_ang_acc; 
                else 
                    new_ang_acc = params_.max_ang_acc * sgn(new_ang_acc); 
 
                new_lin_vel = lin_vel + (new_lin_acc * elapsed); 
                new_ang_vel = ang_vel + (new_ang_acc * elapsed); 
 
                if (abs(new_lin_vel) < params_.max_lin_vel) 
                    lin_vel = new_lin_vel; 
                else 
                    lin_vel = params_.max_lin_vel * sgn(new_lin_vel); 
 
                if (abs(new_ang_vel) < params_.max_ang_vel) 
                    ang_vel = new_ang_vel; 
                else 
                    ang_vel = params_.max_ang_vel * sgn(new_ang_vel); 
            } 
 
            else{ 
                double new_lin_vel = params_.lookahead_lin_vel; 
                double new_ang_vel = curvature * new_lin_vel; 
 
                double new_lin_acc = (new_lin_vel - lin_vel)/elapsed; 
                double new_ang_acc = (new_ang_vel - ang_vel)/elapsed; 
 
                if (abs(new_lin_acc) < params_.max_lin_acc) 
                    new_lin_acc = new_lin_acc; 
                else 
                    new_lin_acc = params_.max_lin_acc * sgn(new_lin_acc); 
                 
                if (abs(new_ang_acc) < params_.max_ang_acc) 
                    new_ang_acc = new_ang_acc; 
                else 
                    new_ang_acc = params_.max_ang_acc * sgn(new_ang_acc); 
 
                new_lin_vel = lin_vel + (new_lin_acc * elapsed); 
                new_ang_vel = ang_vel + (new_ang_acc * elapsed); 
 
                if (abs(new_lin_vel) < params_.max_lin_vel) 
                    lin_vel = new_lin_vel; 
                else 
                    lin_vel = params_.max_lin_vel * sgn(new_lin_vel); 
 
                if (abs(new_ang_vel) < params_.max_ang_vel) 
                    ang_vel = new_ang_vel; 
                else 
                    ang_vel = params_.max_ang_vel * sgn(new_ang_vel); 
            } 
            // ==== end of FIXME ====

            // publish the linear and angular velocities, 
            publishCmdVel(lin_vel, ang_vel);

            // publish the look ahead point for rviz
            publishLookahead(lookahead_point);

        }

        /**
         * Prunes the plan until the last point (closest to the robot) is at least lookahead distance away from the robot.
         * Returns true if there is only one point left on the plan, and cannot be pruned.
         * @param plan Contains at least one point found by the planner.
         */
        bool prunePlan(const V2d &rbt_pos)
        {
            // assert(plan.empty() == false);
            while (plan_.size() > 1)
            {
                const V2d &nearest_point = plan_.back();
                if ((nearest_point - rbt_pos).norm() < params_.lookahead)
                    plan_.pop_back();
                else
                    break;
            }
            return plan_.size() <= 1; // <= is redundancy
        }

        /**
         * Requests a plan from planner_smoother. Fills the plan variable and returns true if request succeeds.
         */
        rclcpp::Client<nav_msgs::srv::GetPlan>::FutureAndRequestId requestPlan(const V2d rbt_pos, const V2d waypoint)
        {
            auto request = std::make_shared<nav_msgs::srv::GetPlan::Request>();
            request->start.pose.position.x = rbt_pos.x;
            request->start.pose.position.y = rbt_pos.y;
            request->goal.pose.position.x = waypoint.x;
            request->goal.pose.position.y = waypoint.y;
            return client_plan_->async_send_request(request);
        }
    };
}