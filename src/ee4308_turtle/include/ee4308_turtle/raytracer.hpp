#include <iostream>
#include <cmath>

#include "ee4308_lib/core.hpp"

#pragma once
namespace ee4308::turtle
{
    /**
     * A general purpose 2D symmetric ray tracer that does not consider rays that are colinear to grid lines.
     * Returns all intermediate cells unlike Bresenham or DDA.
     */
    class RayTracer
    {

    private:
        V2d dif_, len_, from_, to_, grad_;
        V2 root_, sgn_;
        const double REACHED_THRES = 1 - THRES;
        char reached_ = 0;

    public:
        RayTracer(){};
        /**
         * see init().
         */
        RayTracer(const V2d &vertex_from, const V2d &vertex_to) { init(vertex_from, vertex_to); }

        /**
         * Initializes the ray tracer, and returns the root vertex at the starting position.
         * The line will intersect the front cell at the root vertex.
         * The front cell can be obtained using AbstractGrid::adjCellOfVertex(root vertex, this->getSgnDir()).
         * The obstacle_layer.adjCellOfVertex from the Mapper class can be used.
         * Assumes that it is virtually impossible for the ray to travel along the grid lines (and there is no ambiguity in identifying cells).
         * @param vertex_from The grid coordinates where the ray begins.
         * @param vertex_to The grid coordinates where the ray ends.
         */
        const V2 &init(const V2d &vertex_from, const V2d &vertex_to)
        {
            from_ = vertex_from;
            to_ = vertex_to;
            dif_ = to_ - from_;
            for (int d = 0; d < 2; ++d)
            {
                if (dif_(d) - THRES > 0) // THRES may be unnecessary
                {                        // dif_(d) is positive.
                    root_(d) = floor(vertex_from(d));
                    sgn_(d) = 1;
                    grad_(d) = double(sgn_(d)) / dif_(d);
                }
                else if (dif_(d) + THRES < 0) // THRES may be unnecessary
                {                             // dif_(d) is negative
                    root_(d) = ceil(vertex_from(d));
                    sgn_(d) = -1;
                    grad_(d) = double(sgn_(d)) / dif_(d);
                }
                else
                { // dif_(d) is close to zero.
                    root_(d) = floor(vertex_from(d));
                    sgn_(d) = 0;
                    grad_(d) = INFINITY; // set to properly capture
                    dif_(d) = 0; 
                    continue;
                }
            }

            reached_ = 0;
            len_ = 0;
            root_ -= sgn_;

            return next();
        }

        /**
         * Goes to the next root vertex and returns it.
         * Use adjCellOfVertex to get front cell.
         */
        const V2 &next()
        {
            auto getNext = [this](const int &d)
            {
                root_(d) += sgn_(d);
                len_(d) += grad_(d);
                reached_ += (len_(d) > REACHED_THRES);
            };

            if (len_.x - len_.y < -THRES)
                getNext(0); // x crossed a grid line before y crossed a grid line
            else if (len_.x - len_.y > THRES)
                getNext(1); // y crossed a grid line before x crossed a grid line
            else
            { // x and y crossed grid line at same location
                if (len_.x < REACHED_THRES)
                    getNext(0);

                if (len_.y < REACHED_THRES)
                    getNext(1);
            }

            return root_;
        }

        /**
         * Returns the sign of each component in ray's directional vector.
         */
        const V2 &sgnDir() const { return sgn_; }

        /**
         * Returns the root vertex
         */
        const V2 &rootVertex() const { return root_; }

        /**
         * Returns true when the ray has reached the end.
         */
        bool reached() const { return reached_ >= 2; }
    };
}