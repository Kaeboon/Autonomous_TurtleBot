#include <iostream>
#include <algorithm>
#include <cmath>
#include <vector>

#include <rclcpp/rclcpp.hpp>
#include <nav_msgs/msg/occupancy_grid.hpp>
#include <nav_msgs/msg/odometry.hpp>
#include <nav_msgs/srv/get_map.hpp>
#include <sensor_msgs/msg/laser_scan.hpp>
#include <geometry_msgs/msg/pose_stamped.hpp>

#include "ee4308_lib/core.hpp"
#include "ee4308_turtle/raytracer.hpp"
#include "ee4308_turtle/grid.hpp"

#pragma once
namespace ee4308::turtle
{
    struct MapperParameters
    { // contains defaults that can be overwritten
        struct Topics
        {
            std::string pose = "pose";
            std::string scan = "scan";
            std::string obstacle_layer = "obstacle_layer";
            std::string inflation_layer = "inflation_layer";
        } topics;
        struct Services
        {
            std::string get_obstacle_layer = "get_obstacle_layer";
            std::string get_inflation_layer = "get_inflation_layer";
        } services;
        std::string frame_id = "map";    // The parent tf2 frame of the costmaps
        V2 size = {400, 400};            // Number of cells, in {width, height}, which is number of cells in x and y axis respectively.
        V2d origin = {-10, -10};         // World coordinates of the bottom-left corner (-x and -y direction from center) of the grid.
        double resolution = 0.05;        // Length of a cell in meters.
        double min_scan_range = 0.12001; // Minimum scan range for turtlebot's lidar in meters.
        double max_scan_range = 3.49999; // Maximum scan range for turtlebot's lidar in meters.
        double inflation_radius = 0.25;  // Inflation radius in meters.
        double inf_limit = 100;          // The maximum cost on the inflation layer. Between 0 and 100.
        double inflate = 10;             // The amount to increment for an inflation layer cell for every nearby occupied cell in the inflation radius. Between 0 and 100.
        int8_t oc_limit = 100;           // The maximum cost on the obstacle layer. Between 0 and 100.
        int8_t free_limit = 0;           // The minimum cost on the inflation layer. Between 0 and 100.
        int8_t oc_thres = 80;            // Cells on the obstacle layer with a greater or equal cost to this value will be deemed as occupied. Between 0 and 100.
        int8_t free_thres = 20;          // Not currently used. Cells on the obstacle layer with a smaller or equal cost to this value will be deemed as free. Between 0 and 100.
        int8_t occupy = 5;               // The amount to increment if a cell is observed to be occupied, or the amount to decrement if a cell is observed to be free.
        int8_t oc_initial = 50;          // The initial occupancy value for all cells in the obstacle layer.
    };

    /**
     * The Mapper class maintains the inflation and obstacle costmap layers from the lidar of the turtlebot.
     * The costmaps can be exported as a nav_msgs::msg::OccupancyGrid message for publishing.
     */
    class Mapper
    {
    private:
        MapperParameters params_;
        RayTracer ray_tracer_;
        Costmap obstacle_layer_;
        Costmap inflation_layer_;
        Filtermap<int> inflation_filter_;      // to count the number of obstacles inflating each cell.
        Filtermap<size_t> update_filter_;      // the layer containing the latest update id of each cell. Allowed to overflow. 0-255. To prevent multiple updates of a cell with one scan.
        std::vector<Relative> inflation_mask_; // the mask of relative cell coordinates, indices, and values for filling inflation.
        size_t update_id_ = 0;

    public:
        /**
         * Constructor for Mapper using MapperParameters
         * @param params a struct containing all parameters for the Mapper class.
         */
        Mapper(const MapperParameters &params)
            : params_(params),
              obstacle_layer_(params_.size, params_.origin, params_.resolution, params_.oc_initial, params_.frame_id),
              inflation_layer_(params_.size, params_.origin, params_.resolution, 0, params_.frame_id),
              inflation_filter_(params_.size.x * params_.size.y, 0),
              update_filter_(params_.size.x * params_.size.y, 0)
        {
            initInflationMask(params_.inflation_radius, params_.resolution);
        }

    private:
        /**
         * Initializes a circular mask of relative coordinates and cost values, as a list.
         * Each element in the list contains relative coordinates and relative index for a neighboring cell.
         * Add the relative coordinates and relative index to the obstacle cell's to find the absolute coordinates and index of the neighboring cell.
         * More efficient then using a grid of cells as some cells in a grid will not be affected by the inflation.
         * Repurpose this if you want to add an exponentially decaying cost to the inflation mask.
         * @param inflation_radius the radius of the circular inflation mask around an obstacle cell.
         * @param resolution the length of each cell
         */
        void initInflationMask(const double &inflation_radius, const double &resolution)
        {
            double radius = ceil(inflation_radius / resolution); // the mask's radius in terms of the number of cells.
            int radius_ = int(radius);
            std::vector<Relative> sort_by_distance;
            for (int x = -radius_; x <= radius_; ++x)
            {
                for (int y = -radius_; y <= radius_; ++y)
                {
                    double cost = sqrt(x * x + y * y);
                    if (sqrt(x * x + y * y) <= radius)
                    { // assign a lethal cost of 100 to inflated layer if within radius.
                        V2 relative_coordinate(x, y);
                        long relative_idx = obstacle_layer_.cellToIdx(relative_coordinate);
                        sort_by_distance.emplace_back(relative_coordinate, relative_idx, cost);
                    }
                }
            }

            //  sort by distance (for those who wish to extend the inflation layer)
            std::sort(sort_by_distance.begin(), sort_by_distance.end(),
                      [](const Relative &mask1, const Relative &mask2)
                      { return mask1.value < mask2.value; });

            // fill inflation mask
            inflation_mask_ = {};
            for (Relative &mask : sort_by_distance)
            {
                mask.value = params_.inflate; // may need to adjust the value here if the inflation mask is extended.
                inflation_mask_.push_back(mask);
            }
            // if extended, may have to change updateInflationLayer()
        }

        /**
         * Returns true if a cell is already updated by the same scan.
         * @param idx Cell index.
         */
        bool alreadyUpdated(const long &idx)
        {
            if (update_filter_(idx) == update_id_)
                return true;
            update_filter_(idx) = update_id_;
            return false;
        }

        /**
         * Increment the update id.
         */
        void incrementUpdateId() { ++update_id_; }

        /**
         * Updates a cell with an occupancy observation.
         * @param idx Cell index.
         * @param occupy occupancy observation. True for occupied. False for free.
         */
        void updateObstacleLayer(const long &idx, const bool &occupy)
        {
            if (occupy)
            {
                int8_t &obs = obstacle_layer_(idx);
                obs += params_.occupy;
                if (obs > params_.oc_limit)
                    obs = params_.oc_limit;
            }
            else
            {
                int8_t &obs = obstacle_layer_(idx);
                obs -= params_.occupy;
                if (obs < params_.free_limit)
                    obs = params_.free_limit;
            }
        }

        /**
         * Returns true if a cell is occupied.
         * @param idx Cell index.
         */
        bool isOccupied(const long &idx) const { return obstacle_layer_(idx) >= params_.oc_thres; }

        /**
         * Inflates cells surrounding a cell with the inflation mask.
         * @param cell Cell coordinates, must correspond to idx.
         * @param idx Cell index, must correspond to cell.
         * @param inflate True to inflate once around idx. False to deflate once.
         */
        void updateInflationLayer(const V2 &cell, const long &idx, const bool &inflate)
        {
            for (const Relative &mask : inflation_mask_)
            {
                V2 cell_ = cell + mask.rel_cell;
                if (obstacle_layer_.outOfMap(cell_))
                    continue;
                long idx_ = idx + mask.rel_idx; // find the index to increase cost

                inflation_filter_(idx_) += (inflate ? 1 : -1); // count the number of obstacle cells inflating the current cell

                double cost = inflation_filter_(idx_) * mask.value; // costs should always be 0 to LETHAL_COST (<=127)
                if (cost > params_.inf_limit)
                    cost = params_.inf_limit;

                inflation_layer_(idx_) = int8_t(cost);
            }
        }

        /**
         * From an occupancy observation at a cell, updates the obstacle and inflation layers if required.
         * @param cell Cell coordinates, must correspond to idx.
         * @param idx Cell index, must correspond to cell.
         * @param occupy occupancy observation. True for occupied. False for free.
         */
        void tryUpdate(const V2 &cell, const long &idx, const bool &occupy)
        {
            if (alreadyUpdated(idx) == false || (isOccupied(idx) == false && occupy == true))
            { // cell was not updated in the same scan before, or was updated and not occupied and there is and occupy observation.
                bool was_occupied = isOccupied(idx);
                updateObstacleLayer(idx, occupy);
                bool is_occupied = isOccupied(idx);

                if (was_occupied != is_occupied) // update inflation layer if occupancy state changes
                    updateInflationLayer(cell, idx, is_occupied);
            }
        }

    public:
        /**
         * Returns the nav_msgs::msg::OccupancyGrid message of the obstacle layer for publishing.
         */
        nav_msgs::msg::OccupancyGrid msgObstacleLayer() { return obstacle_layer_.msg(); }

        /**
         * Returns the nav_msgs::msg::OccupancyGrid message of the inflation layer for publishing.
         */
        nav_msgs::msg::OccupancyGrid msgInflationLayer() { return inflation_layer_.msg(); }

        /**
         * Updates the obstacle and inflation layers based on a lidar scan and robot pose.
         * @param msg_pose The pose of the robot.
         * @param ranges The LIDAR scan ranges from the sensor_msgs::msg::LaserScan::ranges.
         */
        void updateFromScan(const geometry_msgs::msg::Pose &msg_pose, const std::vector<float> &ranges)
        {
            const double DEG2RAD = M_PI / 180;

            // every new scan will increment update_id_
            incrementUpdateId();

            // for every ray in scan,
            for (int deg = 0; deg < 360; ++deg)
            {
                double range = ranges[deg];

                // ignore ray if range is less than min_range
                if (range < params_.min_scan_range)
                    continue;

                // identify if ray sees an obstacle
                const bool sees_nothing = range > params_.max_scan_range;
                if (sees_nothing)
                    range = params_.max_scan_range;

                // get robot coordinates in world frame
                DOF3 rbt_dof3 = convertPose(msg_pose);
                double ray_heading = rbt_dof3.orientation + deg * DEG2RAD;

                // get end point of lidar ray in world frame
                V2d ray_coord(
                    rbt_dof3.position.x + range * cos(ray_heading),
                    rbt_dof3.position.y + range * sin(ray_heading));

                // convert to map vertices
                V2d rbt_coord = obstacle_layer_.worldToVertex(rbt_dof3.position);
                ray_coord = obstacle_layer_.worldToVertex(ray_coord);

                // initialize ray tracer
                V2 ray_vertex = ray_tracer_.init(rbt_coord, ray_coord);

                // trace the lidar ray
                while (1)
                {
                    V2 ray_cell = AbstractGrid::getFrontCellFromRootVertex(ray_tracer_); // cell in front of root

                    if (obstacle_layer_.outOfMap(ray_cell) == true)
                        break; // skip ray if out of map

                    long ray_idx = obstacle_layer_.cellToIdx(ray_cell);

                    V2 next_ray_vertex = ray_tracer_.next(); // check if next root is at/after the end of ray
                    if (ray_tracer_.reached() == true)
                    {
                        if (sees_nothing == false) // update the last cell as occupied if ray is not longer than max range
                            tryUpdate(ray_cell, ray_idx, true);
                        break;
                    }

                    tryUpdate(ray_cell, ray_idx, false); // have not reached the end

                    ray_vertex = next_ray_vertex; // move to next root
                }
            }
        }
    };

    /**
     * The Mapper ROS Node that maintains subscribers and publishers for the Mapper class.
     */
    class ROSNodeMapper : public rclcpp::Node
    {
    private:
        MapperParameters params_;
        geometry_msgs::msg::Pose msg_pose_;                                              // subscribed message written by callback
        std::vector<float> msg_ranges_;                                                  // subscribed message written by callback
        rclcpp::Subscription<nav_msgs::msg::Odometry>::SharedPtr sub_pose_;              // subscriber
        rclcpp::Subscription<sensor_msgs::msg::LaserScan>::SharedPtr sub_scan_;          // subscriber
        rclcpp::Publisher<nav_msgs::msg::OccupancyGrid>::SharedPtr pub_obstacle_layer_;  // publisher
        rclcpp::Publisher<nav_msgs::msg::OccupancyGrid>::SharedPtr pub_inflation_layer_; // publisher
        rclcpp::Service<nav_msgs::srv::GetMap>::SharedPtr service_obstacle_layer_;       // service
        rclcpp::Service<nav_msgs::srv::GetMap>::SharedPtr service_inflation_layer_;      // service
        std::shared_ptr<Mapper> mapper_;                                                 // the mapper class tied to this node.

    public:
        /**
         * Constructor for the Mapper ROS Node.
         * @param name name of node.
         */
        ROSNodeMapper(
            const std::string &name = "mapper")
            : Node(name)
        {
            // get all parameters from the param server.
            initParams();

            // Initialize topics
            initTopics();

            // Initialize Services
            initServices();
            
            // Initialize the mapper
            mapper_ = std::make_shared<Mapper>(params_);
        }

        /**
         * main run loop
         */
        void run()
        {
            rclcpp::Rate rate(5);

            while (rclcpp::ok())
            {
                mapper_->updateFromScan(msg_pose_, msg_ranges_);

                publishObstacleLayer(); // for rviz

                publishInflationLayer(); // for rviz

                // fill the subscriber callbacks or services.
                rclcpp::spin_some(get_node_base_interface());

                rate.sleep();
            }
        }

    private:
        /**
         * Initialize parameters from the parameter server
         */
        void initParams()
        {
            declare_parameter<std::string>("topics.pose", params_.topics.pose);
            get_parameter<std::string>("topics.pose", params_.topics.pose);
            RCLCPP_INFO_STREAM(get_logger(), "topics.pose: " << params_.topics.pose);

            declare_parameter<std::string>("topics.scan", params_.topics.scan);
            get_parameter<std::string>("topics.scan", params_.topics.scan);
            RCLCPP_INFO_STREAM(get_logger(), "topics.scan: " << params_.topics.scan);

            declare_parameter<std::string>("topics.obstacle_layer", params_.topics.obstacle_layer);
            get_parameter<std::string>("topics.obstacle_layer", params_.topics.obstacle_layer);
            RCLCPP_INFO_STREAM(get_logger(), "topics.obstacle_layer: " << params_.topics.obstacle_layer);

            declare_parameter<std::string>("topics.inflation_layer", params_.topics.inflation_layer);
            get_parameter<std::string>("topics.inflation_layer", params_.topics.inflation_layer);
            RCLCPP_INFO_STREAM(get_logger(), "topics.inflation_layer: " << params_.topics.inflation_layer);

            declare_parameter<std::string>("services.get_obstacle_layer", params_.services.get_obstacle_layer);
            get_parameter<std::string>("services.get_obstacle_layer", params_.services.get_obstacle_layer);
            RCLCPP_INFO_STREAM(get_logger(), "services.get_obstacle_layer: " << params_.services.get_obstacle_layer);

            declare_parameter<std::string>("services.get_inflation_layer", params_.services.get_inflation_layer);
            get_parameter<std::string>("services.get_inflation_layer", params_.services.get_inflation_layer);
            RCLCPP_INFO_STREAM(get_logger(), "services.get_inflation_layer: " << params_.services.get_inflation_layer);

            declare_parameter<std::string>("frame_id", params_.frame_id);
            get_parameter<std::string>("frame_id", params_.frame_id);
            RCLCPP_INFO_STREAM(get_logger(), "frame_id: " << params_.frame_id);

            declare_parameter<int>("size_x", params_.size.x);
            get_parameter<int>("size_x", params_.size.x);
            RCLCPP_INFO_STREAM(get_logger(), "size_x: " << params_.size.x);

            declare_parameter<int>("size_y", params_.size.y);
            get_parameter<int>("size_y", params_.size.y);
            RCLCPP_INFO_STREAM(get_logger(), "size_y: " << params_.size.y);

            declare_parameter<double>("origin_x", params_.origin.x);
            get_parameter<double>("origin_x", params_.origin.x);
            RCLCPP_INFO_STREAM(get_logger(), "origin_x: " << params_.origin.x);

            declare_parameter<double>("origin_y", params_.origin.y);
            get_parameter<double>("origin_y", params_.origin.y);
            RCLCPP_INFO_STREAM(get_logger(), "origin_y: " << params_.origin.y);

            declare_parameter<double>("resolution", params_.resolution);
            get_parameter<double>("resolution", params_.resolution);
            RCLCPP_INFO_STREAM(get_logger(), "resolution: " << params_.resolution);

            declare_parameter<double>("min_scan_range", params_.min_scan_range);
            get_parameter<double>("min_scan_range", params_.min_scan_range);
            RCLCPP_INFO_STREAM(get_logger(), "min_scan_range: " << params_.min_scan_range);

            declare_parameter<double>("max_scan_range", params_.max_scan_range);
            get_parameter<double>("max_scan_range", params_.max_scan_range);
            RCLCPP_INFO_STREAM(get_logger(), "max_scan_range: " << params_.max_scan_range);

            declare_parameter<double>("inflation_radius", params_.inflation_radius);
            get_parameter<double>("inflation_radius", params_.inflation_radius);
            RCLCPP_INFO_STREAM(get_logger(), "inflation_radius: " << params_.inflation_radius);

            declare_parameter<double>("inf_limit", params_.inf_limit);
            get_parameter<double>("inf_limit", params_.inf_limit);
            RCLCPP_INFO_STREAM(get_logger(), "inf_limit: " << params_.inf_limit);

            declare_parameter<double>("inflate", params_.inflate);
            get_parameter<double>("inflate", params_.inflate);
            RCLCPP_INFO_STREAM(get_logger(), "inflate: " << params_.inflate);

            declare_parameter<int8_t>("oc_limit", params_.oc_limit);
            get_parameter<int8_t>("oc_limit", params_.oc_limit);
            RCLCPP_INFO_STREAM(get_logger(), "oc_limit: " << int(params_.oc_limit));

            declare_parameter<int8_t>("free_limit", params_.free_limit);
            get_parameter<int8_t>("free_limit", params_.free_limit);
            RCLCPP_INFO_STREAM(get_logger(), "free_limit: " << int(params_.free_limit));

            declare_parameter<int8_t>("oc_thres", params_.oc_thres);
            get_parameter<int8_t>("oc_thres", params_.oc_thres);
            RCLCPP_INFO_STREAM(get_logger(), "oc_thres: " << int(params_.oc_thres));

            declare_parameter<int8_t>("free_thres", params_.free_thres);
            get_parameter<int8_t>("free_thres", params_.free_thres);
            RCLCPP_INFO_STREAM(get_logger(), "free_thres: " << int(params_.free_thres));

            declare_parameter<int8_t>("occupy", params_.occupy);
            get_parameter<int8_t>("occupy", params_.occupy);
            RCLCPP_INFO_STREAM(get_logger(), "occupy: " << int(params_.occupy));

            declare_parameter<int8_t>("oc_initial", params_.oc_initial);
            get_parameter<int8_t>("oc_initial", params_.oc_initial);
            RCLCPP_INFO_STREAM(get_logger(), "oc_initial: " << int(params_.oc_initial));
        }

        /**
         * Initialize subscribers and wait until messages from all subscribed topics arrive.
         */
        void initTopics()
        {
            // Initialize publishers
            pub_obstacle_layer_ = create_publisher<nav_msgs::msg::OccupancyGrid>(params_.topics.obstacle_layer, 1);
            pub_inflation_layer_ = create_publisher<nav_msgs::msg::OccupancyGrid>(params_.topics.inflation_layer, 1);

            // Initialize messages with values that will never be written by their publishers.
            msg_ranges_ = {};
            msg_pose_.position.z = NAN;

            // Initialize subscribers
            sub_pose_ = create_subscription<nav_msgs::msg::Odometry>(
                params_.topics.pose, 1, std::bind(&ROSNodeMapper::subscriberCallbackPose, this, std::placeholders::_1));

            sub_scan_ = create_subscription<sensor_msgs::msg::LaserScan>(
                params_.topics.scan, rclcpp::SensorDataQoS(), std::bind(&ROSNodeMapper::subscriberCallbackScan, this, std::placeholders::_1));

            // Wait for messages to arrive.
            rclcpp::Rate rate(5);
            while (rclcpp::ok() && (msg_ranges_.empty() || std::isnan(msg_pose_.position.z)))
            {
                // RCLCPP_INFO_STREAM(get_logger(), "Waiting for topics...");
                rclcpp::spin_some(get_node_base_interface());
                rate.sleep();
            }
            RCLCPP_INFO_STREAM(get_logger(), "Finished waiting for topics.");
        }

        /** Initialize service */
        void initServices()
        {
            service_obstacle_layer_ = create_service<nav_msgs::srv::GetMap>(
                params_.services.get_obstacle_layer, std::bind(&ROSNodeMapper::serviceObstacleLayer, this, std::placeholders::_1, std::placeholders::_2));
            service_inflation_layer_ = create_service<nav_msgs::srv::GetMap>(
                params_.services.get_inflation_layer, std::bind(&ROSNodeMapper::serviceInflationLayer, this, std::placeholders::_1, std::placeholders::_2));

            RCLCPP_INFO_STREAM(get_logger(), "Finished services.");
        }

        /**
         * The LIDAR scan topic's subscriber callback
         */
        void subscriberCallbackScan(sensor_msgs::msg::LaserScan msg) { msg_ranges_ = msg.ranges; }

        /**
         * The robot pose topic's subscriber callback
         */
        void subscriberCallbackPose(const nav_msgs::msg::Odometry &msg) { msg_pose_ = msg.pose.pose; }

        /**
         * The service callback to respond with the obstacle layer.
         */
        void serviceObstacleLayer(const std::shared_ptr<nav_msgs::srv::GetMap::Request> request,
                                  std::shared_ptr<nav_msgs::srv::GetMap::Response> response)
        {
            (void)(request); // suppress compiler unused warnings.
            response->map = mapper_->msgObstacleLayer();
        }

        /**
         * The service callback to respond with the inflation layer.
         */
        void serviceInflationLayer(const std::shared_ptr<nav_msgs::srv::GetMap::Request> request,
                                   std::shared_ptr<nav_msgs::srv::GetMap::Response> response)
        {
            (void)(request); // suppress compiler unused warnings.
            response->map = mapper_->msgInflationLayer();
        }

        /**
         * Publishes the obstacle layer.
         * @param msg_obstacle_layer the message containing the obstacle layer from the Mapper class.
         */
        void publishObstacleLayer()
        {
            nav_msgs::msg::OccupancyGrid msg = mapper_->msgObstacleLayer();
            msg.header.stamp = now();
            pub_obstacle_layer_->publish(msg);
        }

        /**
         * Publishes the inflation layer.
         * @param msg_obstacle_layer the message containing the inflation layer from the Mapper class.
         */
        void publishInflationLayer()
        {
            nav_msgs::msg::OccupancyGrid msg = mapper_->msgInflationLayer();
            msg.header.stamp = now();
            pub_inflation_layer_->publish(msg);
        }
    };
}