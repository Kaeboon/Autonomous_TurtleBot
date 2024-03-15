#include <iostream>
#include <algorithm>
#include <cmath>
#include <chrono>
#include <vector>
#include <list>
#include <array>
#include <mutex>

#include <rclcpp/rclcpp.hpp>
#include <nav_msgs/msg/occupancy_grid.hpp>
#include <nav_msgs/msg/path.hpp>
#include <nav_msgs/msg/odometry.hpp>
#include <nav_msgs/srv/get_map.hpp>
#include <nav_msgs/srv/get_plan.hpp>
#include <geometry_msgs/msg/pose_stamped.hpp>

#include "ee4308_lib/core.hpp"
#include "ee4308_turtle/raytracer.hpp"
#include "ee4308_turtle/grid.hpp"

#pragma once
namespace ee4308::turtle
{
    struct PlannerSmootherParameters
    { // contains defaults that can be overwritten
        struct Topics
        {
            std::string plan = "plan";
        } topics;
        struct Services
        {
            std::string get_inflation_layer = "get_inflation_layer";
            std::string get_plan = "get_plan";
        } services;
        std::string frame_id = "map";
        double spline_vel = 0.2;
        double time_step = 0.1;
    };

    struct PlannerNode
    {
        PlannerNode *parent = nullptr;
        V2 cell = {0, 0};
        double cost_h = INFINITY;
        double cost_g = INFINITY;
        double cost_f = INFINITY;
        bool expanded = false;
    };

    /**
     * Implements the open list with insert sort.
     */
    class OpenList
    { // insert sort. reasonably simple and fast.
    private:
        std::list<PlannerNode *> list_;

    public:
        /**
         * Enqueues a node based on its f-cost.
         */
        void queue(PlannerNode *const &node)
        {
            auto node_it = list_.begin();
            for (; node_it != list_.end(); node_it = std::next(node_it))
                if (node->cost_f < (*node_it)->cost_f)
                    break;
            list_.insert(node_it, node);
        }

        /**
         * Polls the cheapest f-cost node.
         */
        PlannerNode *poll()
        {
            PlannerNode *front_node = list_.front();
            list_.pop_front();
            return front_node;
        }

        /**
         * Returns true if no more nodes.
         */
        bool empty() const { return list_.empty(); }

        /**
         * Clears the open list
         */
        void clear() { list_.clear(); }
    };

    /**
     * The PlannerSmoother class. Is Dijkstra. Convert to A*.
     * Additionally smooths the planner's path into a more feasible trajectory.
     */
    class PlannerSmoother
    {
    private:
        PlannerSmootherParameters params_;
        // RayTracer ray_tracer_; // for Theta*. Note that the center of cells have grid coordinates (##.5, ##.5).

        Costmap inflation_layer_;
        OpenList open_list_;

        std::vector<V2d> path_; // in world coordinates
        std::array<Relative, 8> neighbor_lut_;

    public:
        /**
         * Constructor for PlannerSmoother using PlannerSmootherParameters
         * @param params a struct containing all parameters for the Planner class.
         */
        PlannerSmoother(const PlannerSmootherParameters &params)
            : params_(params)
        {
            // Create a look-up table to store the relative coordinates, indices, and costs for the neighboring cells.
            for (int h = 0; h < 8; ++h)
            {
                Relative &neighbor = neighbor_lut_[h];
                neighbor.rel_cell = headingToDirection(h);
                // neighbor.rel_idx // not required.
                neighbor.value = (h & 1) == 0 ? 1 : std::sqrt(2); // if even, put 1. If odd, put sqrt(2).
            }
        }

    private:
        /**
         * Fills the path (in world coordinates) once the expanded node lies on the goal cell
         * The function will write at least the original goal coordinate: [goal_coord, ...],
         * followed by the world coordinates at the center of the cells that lie along the path.
         * @param expanded_node the expanded node
         * @param goal_coord the goal world coordinates
         */
        void foundPath(PlannerNode *expanded_node, const V2d &goal_coord)
        {
            PlannerNode *node = expanded_node;
            // std::cout << "Path Found: { ";

            // fill the goal coord (based on the original coordinates)
            path_.push_back(goal_coord);
            // std::cout << path_.back() << "; ";

            // fill the coords, if any
            do
            {
                path_.push_back(inflation_layer_.cellToWorld(node->cell, true));
                // std::cout << path_.back() << "; ";
                node = node->parent;
            } while (node != nullptr);

            // std::cout << "}" << std::endl;
        }

    public:
        /**
         * Finds a path between the start and goal world frame coordinates.
         * The returned path is *reversed*.
         * The goal point is at the beginning of the path, and the start point is at the back.
         * Assumes that the map is already updated with updateCostMap.
         * @param start_coord world coordinates to search from.
         * @param goal_coord world coordinates to search to.
         */
        const std::vector<V2d> &run(const V2d &start_coord, const V2d &goal_coord)
        {
            // clear path
            path_.clear();

            // Convert world coordinates to cell coordinates
            V2 start_cell = inflation_layer_.worldToCell(start_coord);
            V2 goal_cell = inflation_layer_.worldToCell(goal_coord);

            // Initialize nodes with the same size as the inflation layer
            std::vector<PlannerNode> nodes(inflation_layer_.size().x * inflation_layer_.size().y);

            { // Initialize the start node and queue into open list
                long idx = inflation_layer_.cellToIdx(start_cell);
                PlannerNode *node = &nodes[idx];
                node->cost_g = 0;
                node->cost_h = sqrt(pow((goal_cell.y-start_cell.y),2) + pow((goal_cell.x-start_cell.x),2));
                node->cost_f = node->cost_g + node->cost_h;
                node->parent = NULL;
                node->cell = start_cell;
                open_list_.queue(node);
            }

            // Loop until path is found or open_list is empty.
            while (open_list_.empty() == false && rclcpp::ok())
            {
                PlannerNode *const expanded_node = open_list_.poll();

                // skip if in closed list.
                if (expanded_node->expanded == true)
                    continue;
                expanded_node->expanded = true;

                // return path if expanded_node is the goal node
                if (expanded_node->cell == goal_cell)
                { // goal found. return path
                    foundPath(expanded_node, goal_coord);
                    break; // break to clear open_list
                }

                // search the neighbors
                for (const Relative &neighbor : neighbor_lut_)
                {
                    // get neighbor's cell coordinate
                    V2 neighbor_cell = expanded_node->cell + neighbor.rel_cell;

                    // skip neighbor node if out of map
                    if (inflation_layer_.outOfMap(neighbor_cell))
                        continue;

                    // get neighbor's index
                    long neighbor_idx = inflation_layer_.cellToIdx(neighbor_cell);

                    // get neighbor node.
                    PlannerNode *const neighbor_node = &nodes[neighbor_idx];

                    // skip if neighbor is already in closed list (has to be cheaper than expanded node)
                    if (neighbor_node->expanded == true)
                        continue;

                    // find the cost to reach the neighbor from the current node, with extra costs from the inflation layer.
                    double diff_g = neighbor.value * (inflation_layer_(neighbor_idx) + 1); // add 1 to avoid zero costs.
                    double test_g = expanded_node->cost_g + diff_g;

                    // find heuristic cost h
                    double test_h = sqrt(pow((goal_cell.y-neighbor_cell.y),2) + pow((goal_cell.x-neighbor_cell.x),2));

                    // queue neighbor node to open list and write data to if test_g is the cheapest so far.
                    if (test_g < neighbor_node->cost_g)
                    {
                        neighbor_node->cell = neighbor_cell; // cell coordinate was not initialized
                        neighbor_node->cost_g = test_g;
                        neighbor_node->cost_h = test_h; // FIXME
                        neighbor_node->cost_f = neighbor_node->cost_g + neighbor_node->cost_h; // FIXME
                        neighbor_node->parent = expanded_node;

                        open_list_.queue(neighbor_node);
                    }
                }
            }

            // Clear open list
            open_list_.clear();
            
            return path();
        }

        /**
         * Smooths the path into a more feasible trajectory
         */
        const std::vector<V2d> &smooth() // FIXME
        {
            // get the start and goal coord from the path
            V2d start_coord = path_[0];
            V2d goal_coord = path_[path_.size()-1];

            // create an array to store the smooth path and smooth path vel
            std::vector<V2d> smooth_path_;
            std::vector<V2d> smooth_path_vel_;

            // loop through all the paths from the planner 
             for (size_t i = 0; i < path_.size(); ++i) 
            { 
                V2d coord = path_[i]; 
                
                // directly queue the path if it is start or goal coord
                if (coord == start_coord || coord == goal_coord){ 
                    smooth_path_.push_back(coord);
                    smooth_path_vel_.push_back(V2d(0,0));
                    continue; 
                } 
                 
                else{ 
                    // cross product to check whether the points are parallel to each other
                    V2d coordprev = path_[i-1]; 
                    V2d coordnext = path_[i+1]; 
                    V2d segmentone = V2d(coord.x - coordprev.x, coord.y - coordprev.y); 
                    V2d segmenttwo = V2d(coordnext.x - coord.x, coordnext.y - coord.y); 
                    double crossproduct = (segmentone.x * segmenttwo.y) - (segmentone.y * segmenttwo.x); 
                    // queue the coords of segments that are not parallel to each other and find their velocities and queue
                    if (crossproduct!=0){ 
                        smooth_path_.push_back(coord);
                        double segmentonelength = sqrt(segmentone.x * segmentone.x + segmentone.y * segmentone.y);
                        double segmenttwolength = sqrt(segmenttwo.x * segmenttwo.x + segmenttwo.y * segmenttwo.y);
                        V2d unisegmentone = V2d(segmentone.x / segmentonelength, segmentone.y / segmentonelength);
                        V2d unisegmenttwo = V2d(segmenttwo.x / segmenttwolength, segmenttwo.y / segmenttwolength);
                        V2d avgsegment = V2d((unisegmentone.x+unisegmenttwo.x)/2,(unisegmentone.y+unisegmenttwo.y)/2);
                        double avgsegmentlength = sqrt(avgsegment.x * avgsegment.x + avgsegment.y * avgsegment.y);
                        smooth_path_vel_.push_back(V2d(params_.spline_vel * avgsegment.x / avgsegmentlength, params_.spline_vel * avgsegment.y / avgsegmentlength));
                    }  
                } 
            }

            std::vector<V2d> final_smooth_path_;

            for (size_t i = 0; i < smooth_path_.size()-1; ++i)
            {
                //calculate segment length and tf
                double segmentlength = sqrt((smooth_path_[i+1].x - smooth_path_[i].x)*(smooth_path_[i+1].x - smooth_path_[i].x) + (smooth_path_[i+1].y - smooth_path_[i].y)*(smooth_path_[i+1].y - smooth_path_[i].y));
                const double avg_vel = 0.16;
                double tf = segmentlength / avg_vel;
                double timestep = params_.time_step;
                
                //calculate the coefficients of the spline equations
                double a0 = smooth_path_[i].x;
                double a1 = smooth_path_vel_[i].x;
                double a2 = (-3/pow(tf,2))*smooth_path_[i].x - (2/tf)*smooth_path_vel_[i].x + (3/pow(tf,2))*smooth_path_[i+1].x - (1/tf)*smooth_path_vel_[i+1].x;
                double a3 = (2/pow(tf,3))*smooth_path_[i].x + (1/pow(tf,2))*smooth_path_vel_[i].x - (2/pow(tf,3))*smooth_path_[i+1].x + (1/pow(tf,2))*smooth_path_vel_[i+1].x;

                double b0 = smooth_path_[i].y;
                double b1 = smooth_path_vel_[i].y;
                double b2 = (-3/pow(tf,2))*smooth_path_[i].y - (2/tf)*smooth_path_vel_[i].y + (3/pow(tf,2))*smooth_path_[i+1].y - (1/tf)*smooth_path_vel_[i+1].y;
                double b3 = (2/pow(tf,3))*smooth_path_[i].y + (1/pow(tf,2))*smooth_path_vel_[i].y - (2/pow(tf,3))*smooth_path_[i+1].y + (1/pow(tf,2))*smooth_path_vel_[i+1].y;

                //loop through small timestep until tf 
                for (double j = 0; j <= tf; j+=timestep){
                    double dt = j;
                    // substitute the dt into the spline equation to get the interpolated points 
                    V2d coord_intermediate(a0 + a1*dt + a2 * pow(dt, 2) + a3 * pow(dt, 3), b0 + b1*dt + b2*pow(dt, 2) + b3*pow(dt, 3));
                    final_smooth_path_.push_back(coord_intermediate);
                }
            }

            // replace the path_ with the smooth_path
            path_ = final_smooth_path_;

            // returns path_
            return path(); 
        }

        /**
         * Updates the local copy of the cost map (inflation layer) to search the paths on.
         */
        void updateCostMap(const nav_msgs::msg::OccupancyGrid &msg) { inflation_layer_.copyFrom(msg); }

        /**
         * Returns the path.
         */
        const std::vector<V2d> &path() const { return path_; }

        /**
         * Returns the path as a nav_msgs::msg::Path message.
         */
        nav_msgs::msg::Path msg() const
        {
            nav_msgs::msg::Path msg_;
            msg_.header.frame_id = params_.frame_id;

            for (const V2d &coord : path())
            {
                geometry_msgs::msg::PoseStamped pose;
                pose.pose.position.x = coord.x;
                pose.pose.position.y = coord.y;
                pose.pose.orientation.w = 1;
                msg_.poses.push_back(pose);
            }

            return msg_;
        }
    };

    /**
     * The Planner ROS Node that maintains subscribers and publishers for the Planner class.
     */
    class ROSNodePlannerSmoother : public rclcpp::Node
    {
    private:
        PlannerSmootherParameters params_;
        rclcpp::Publisher<nav_msgs::msg::Path>::SharedPtr pub_path_;              // publisher
        rclcpp::Client<nav_msgs::srv::GetMap>::SharedPtr client_inflation_layer_; // client
        rclcpp::Service<nav_msgs::srv::GetPlan>::SharedPtr service_plan_;         // service
        std::shared_ptr<PlannerSmoother> planner_smoother_;                       // the planner and smoothing class tied to this node.
        rclcpp::CallbackGroup::SharedPtr cb_group_;                               // to allow all callbacks to simultaneously occur in the executor. Requires node to be added to MultiThreaderExecutor
        std::mutex mutex_;                                                        // required to prevent data races

    public:
        /**
         * Constructor for the Planner ROS Node.
         * @param name name of node.
         */
        ROSNodePlannerSmoother(
            const std::string &name = "planner_smoother")
            : Node(name)
        {
            // Initialize call back group
            cb_group_ = create_callback_group(rclcpp::CallbackGroupType::Reentrant);

            // get all parameters from the param server.
            initParams();

            // Initialize topics
            initTopics();

            // Initialize Services
            initServices();

            // Initialize the planner
            planner_smoother_ = std::make_shared<PlannerSmoother>(params_);
        }

    private:
        /**
         * Initialize parameters from the parameter server
         */
        void initParams()
        {
            declare_parameter<std::string>("topics.plan", params_.topics.plan);
            get_parameter<std::string>("topics.plan", params_.topics.plan);
            RCLCPP_INFO_STREAM(get_logger(), "topics.plan: " << params_.topics.plan);

            declare_parameter<std::string>("services.get_inflation_layer", params_.services.get_inflation_layer);
            get_parameter<std::string>("services.get_inflation_layer", params_.services.get_inflation_layer);
            RCLCPP_INFO_STREAM(get_logger(), "services.get_inflation_layer: " << params_.services.get_inflation_layer);

            declare_parameter<std::string>("services.get_plan", params_.services.get_plan);
            get_parameter<std::string>("services.get_plan", params_.services.get_plan);
            RCLCPP_INFO_STREAM(get_logger(), "services.get_plan: " << params_.services.get_plan);

            declare_parameter<std::string>("frame_id", params_.frame_id);
            get_parameter<std::string>("frame_id", params_.frame_id);
            RCLCPP_INFO_STREAM(get_logger(), "frame_id: " << params_.frame_id);

            declare_parameter<double>("spline_vel", params_.spline_vel);
            get_parameter<double>("spline_vel", params_.spline_vel);
            RCLCPP_INFO_STREAM(get_logger(), "spline_vel: " << params_.spline_vel);

            declare_parameter<double>("time_step", params_.time_step);
            get_parameter<double>("time_step", params_.time_step);
            RCLCPP_INFO_STREAM(get_logger(), "time_step: " << params_.time_step);
        }

        /**
         * Initialize subscribers and wait until messages from all subscribed topics arrive.
         */
        void initTopics()
        {
            // Initialize Publishers
            pub_path_ = create_publisher<nav_msgs::msg::Path>(params_.topics.plan, 1);
        }

        /**
         * Initialize services and wait for them to respond
         */
        void initServices()
        {
            client_inflation_layer_ = create_client<nav_msgs::srv::GetMap>(
                params_.services.get_inflation_layer,
                rmw_qos_profile_services_default, cb_group_);
            service_plan_ = create_service<nav_msgs::srv::GetPlan>(
                params_.services.get_plan,
                std::bind(&ROSNodePlannerSmoother::servicePlan, this, std::placeholders::_1, std::placeholders::_2),
                rmw_qos_profile_services_default, cb_group_);

            // wait for service to respond
            while (rclcpp::ok() == true &&
                   client_inflation_layer_->wait_for_service(std::chrono::duration<int, std::milli>(200)) == false) // wait for 200ms
            {
                // RCLCPP_INFO_STREAM(get_logger(), "Waiting for service servers...");
            }

            RCLCPP_INFO_STREAM(get_logger(), "Finished services.");
        }

        /**
         * Requests a map from mapper. Returns true if a map is received and copied.
         */
        bool requestInflationLayer()
        {
            // Wait for the service to respond.

            // Send the request.
            auto request = std::make_shared<nav_msgs::srv::GetMap::Request>();
            auto result = client_inflation_layer_->async_send_request(request);

            // wait for timeout
            while (rclcpp::ok() == true && result.future.valid() == false)
            { // wait until it responds.
            }

            if (rclcpp::ok() == false)
                return false;

            nav_msgs::msg::OccupancyGrid msg = result.get()->map;

            // update the internal map in the planner.
            planner_smoother_->updateCostMap(msg);

            return true;
        }

        /**
         * The service callback to respond with the path
         */
        void servicePlan(const std::shared_ptr<nav_msgs::srv::GetPlan::Request> request,
                         std::shared_ptr<nav_msgs::srv::GetPlan::Response> response)
        {
            // wait until a lock is acquired. Released when this function returns.
            const std::lock_guard<std::mutex> lock(mutex_);

            // request a map. If shutdown or failed, return the default plan (nothing).
            if (requestInflationLayer() == false)
            {
                std::cout << "Inflation layer cannot be requested. No path returned." << std::endl;
                return;
            }

            // retrieve the request's start and goal coordinates
            V2d start_coord = {request->start.pose.position.x, request->start.pose.position.y};
            V2d goal_coord = {request->goal.pose.position.x, request->goal.pose.position.y};

            // find the shortest path and store the path within the class.
            std::cout << "Running Planner..." << std::endl;
            planner_smoother_->run(start_coord, goal_coord);
            std::cout << "Run complete." << std::endl;

            // smooths the path / generate smooth trajectory
            planner_smoother_->smooth();

            // get and stamp the message
            nav_msgs::msg::Path msg = planner_smoother_->msg();
            msg.header.stamp = now();

            // publish the path
            pub_path_->publish(msg);

            // respond with the path
            response->plan = msg;
        }
    };
}