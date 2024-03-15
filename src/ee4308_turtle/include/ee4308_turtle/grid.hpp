#include <iostream>
#include <cmath>

#include <rclcpp/rclcpp.hpp>
#include <nav_msgs/msg/occupancy_grid.hpp>
#include <eigen3/Eigen/Core>

#include "ee4308_lib/core.hpp"
#include "ee4308_turtle/raytracer.hpp"

#pragma once
namespace ee4308::turtle
{
    class AbstractGrid
    {
    protected:
        V2 size_;
        V2d origin_;
        double resolution_;
        int8_t *data_;

    public:
        AbstractGrid() : size_(0, 0), origin_(0, 0), resolution_(0), data_(nullptr) {}

        ~AbstractGrid()
        {
            if (data_ != nullptr)
                delete[] data_;
        }

        AbstractGrid(const V2 &size, const V2d &origin, const double &resolution, const int8_t &initial)
            : size_(size), origin_(origin), resolution_(resolution)
        {
            size_t len = size.x * size.y;
            data_ = {new int8_t[len]};
            for (size_t i = 0; i < len; ++i)
                data_[i] = initial;
        }

        /**
         * Returns the size = (width, height) = (num cells in x, num cells in y) of the map.
         */
        V2 size() const { return size_; }

        /**
         * Returns the origin coordinates (meters) in world frame.
         */
        const V2d &origin() const { return origin_; }

        /**
         * Returns the grid resolution (meters), which is the cell length.
         */
        const double &resolution() const { return resolution_; }

        /**
         * Converts world coordinates to grid coordinates (in terms of number of cells).
         * @param coords World coordinates (meters).
         */
        V2d worldToVertex(const V2d &coords) { return (coords - origin()) / resolution(); }

        /**
         * Converts grid coordinates to world coordinates.
         * @param vertex Grid coordinates (number of cells).
         */
        V2d vertexToWorld(const V2d &vertex) { return vertex * resolution() + origin(); }

        /**
         * Converts world coordinates to the corresponding cell coordinates. A cell's cell coordinates have the same value as the grid coordinates at the cell's bottom-left corner.
         * @param coords World coordinates (meters)
         */
        V2 worldToCell(const V2d &coords) { return worldToVertex(coords).floor(); }

        /**
         * Converts cell coordinates to world coordinates.
         * @param cell Cell coordinates. Has same value as the grid coordinates at the bottom-left corner of cell.
         * @param center If true, return the world coordinates at the center of the cell. If false, return the world corodinates at the bottom left.
         */
        V2d cellToWorld(const V2 &cell, const bool &center = false)
        {
            if (center == true)
                return vertexToWorld(cell) + (resolution() / 2);
            else
                return vertexToWorld(cell);
        }

        /**
         * Converts cell coordinates to a flattened index for accessing cost data in underlying containers.
         * @param cell Cell coordinates. Has same value as the grid coordinates at the cell's bottom-left corner.
         */
        int cellToIdx(const V2 &cell) { return cell.y * size().x + cell.x; }

        /**
         * Converts flattened cell index to cell coordinates.
         * @param idx Flattened index.
         */
        V2 idxToCell(const int &idx) { return {idx % size().x, idx / size().x}; }

        /**
         * Returns true if out of the map.
         * @param cell Cell coordinates.
         */
        bool outOfMap(const V2 &cell) { return cell.x < 0 || cell.x >= size().x || cell.y < 0 || cell.y >= size().y; }

        /**
         * At a cell coordinate (bottom-left corner of a cell), find the cell coordinates of an adjacent cell.
         * Imagine a 2x2 checkerboard, with the cell coordinate at the center.
         * Returns the cell coordinate of a cell in the checkerboard that lies in the desired direction.
         * Behavior for cardinal directions (one component in direction is zero) not defined.
         * @param cell Cell coordinates.
         * @param direction Directional vector of adjacent cell, pointing away from the first parameter (cell).
         */
        static V2 adjCellOfVertex(const V2 &cell, V2 direction)
        {
            direction.x = direction.x > 0 ? 0 : -1;
            direction.y = direction.y > 0 ? 0 : -1;
            return cell + direction;
        }

        /**
         * Retrieves the cell coordinates in front of the root vertex returned by the RayTracer class.
         * @param ray_tracer RayTracer object
         */
        static V2 getFrontCellFromRootVertex(const RayTracer &ray_tracer) { return adjCellOfVertex(ray_tracer.rootVertex(), ray_tracer.sgnDir()); }
    };

    template <typename data_t>
    class Filtermap
    {
    private:
        data_t *data_;
        size_t size_;

    public:
        /**
         * A lightweight wrapper over a data_t* array to implement a filter/mask over a map.
         * Has no validations or error checks, so there is segmentation fault if idx does not fall in range 0 <= idx < length.
         * If not sized properly, can cause node to fail without log INFO output.
         * @param size the total number of cells
         * @param initial Initial value of all cells.
         */
        Filtermap(const size_t &size = 40000, const int8_t &initial = 0)
            : size_(size)
        {
            data_ = {new data_t[size]};
            for (size_t i = 0; i < size; ++i)
                data_[i] = initial;
        }

        /**
         * Returns the number of cells in the filtermap
         */
        const size_t &size() const { return size_; }

        /**
         * Returns the data stored in a grid cell.
         * @param idx Flattened index of grid cell. 0 <= idx < size.
         */
        const data_t &operator()(const int &idx) const { return data_[idx]; }

        /**
         * Returns the data stored in a grid cell.
         * @param idx Flattened index of grid cell. 0 <= idx < size.
         */
        data_t &operator()(const int &idx) { return data_[idx]; }
    };

    class Costmap : public AbstractGrid
    {
    private:
        // nav_msgs::msg::OccupancyGrid msg_; // store the map in here so we have slightly less overheads when publishing. and it is simpler to code.
        std::string frame_id;

    public:
        /** Default constructor */
        Costmap() : AbstractGrid(), frame_id("") {}

        /**
         * Costmap is an Abstract Grid that can convert internal data to occupancy grid message.
         * @param frame_id Parent tf2 frame id.
         * @param size {width, height}, where width and height is the number of cells along x-axis and y-axis respectively.
         * @param resolution Length of cell (square) in meters.
         * @param origin World frame coordinates (meters) of the bottom-left corner of the grid. Bottom-left is -x and -y direction from center. Bottom-left of cell at index (xk,yk) is at the grid vertex (xk,yk). Grid vertex (xk,yk) is (origin + resolution*(xk,yk)) in world frame.
         * @param initial Initial value of all cells.
         */
        Costmap(const V2 &size,
                const V2d &origin,
                const double &resolution,
                const int8_t &initial,
                const std::string &frame_id)
            : AbstractGrid(size, origin, resolution, initial), frame_id(frame_id) {}

    public:
        /**
         * Returns the nav_msgs::msg::OccupancyGrid message without timestamping.
         */
        nav_msgs::msg::OccupancyGrid msg()
        {
            nav_msgs::msg::OccupancyGrid msg_;
            msg_.header.frame_id = frame_id;
            msg_.info.height = size().y;
            msg_.info.width = size().x;
            msg_.info.resolution = resolution();
            msg_.info.origin.orientation.w = 1;
            msg_.info.origin.orientation.x = 0;
            msg_.info.origin.orientation.y = 0;
            msg_.info.origin.orientation.z = 0;
            msg_.info.origin.position.x = origin().x;
            msg_.info.origin.position.y = origin().y;
            msg_.info.origin.position.z = 0;
            msg_.data.resize(msg_.info.height * msg_.info.width);

            for (size_t i = 0; i < msg_.data.size(); ++i)
                msg_.data[i] = data_[i];

            return msg_;
        }

        /**
         * Returns the data stored in a grid cell.
         * @param idx Flattened index of grid cell.
         */
        const int8_t &operator()(const int &idx) const { return data_[idx]; }

        /**
         * Returns the data stored in a grid cell.
         * @param idx Flattened index of grid cell.
         */
        int8_t &operator()(const int &idx) { return data_[idx]; }

        void copyFrom(const nav_msgs::msg::OccupancyGrid &new_msg)
        {
            // initialize the class if not done so.
            if (data_ == nullptr)
            {
                size_ = V2(new_msg.info.width, new_msg.info.height);
                origin_ = V2d(new_msg.info.origin.position.x, new_msg.info.origin.position.y);
                resolution_ = new_msg.info.resolution;
                frame_id = new_msg.header.frame_id;
                size_t len = size().x * size().y;
                data_ = {new int8_t[len]};
            }

            for (size_t i = 0; i < new_msg.data.size(); ++i)
                data_[i] = new_msg.data[i];
        }
    };

    struct Relative
    {
        V2 rel_cell;
        int rel_idx;
        double value;

        Relative() : rel_cell({0, 0}), rel_idx(0), value(0) {}

        Relative(const V2 &rel_cell, const int &rel_idx, const double &value)
            : rel_cell(rel_cell), rel_idx(rel_idx), value(value) {}
    };
}