#include <iostream>
#include <iomanip>

#include "eigen3/Eigen/Core"
#include "ee4308_lib/common.hpp"

#pragma once
namespace ee4308
{
    template <typename T>
    class Vec2
    {
    public:
        static const size_t DIM_ = 2;

        T x, y;
        Vec2(const T &x, const T &y) : x(x), y(y) {}
        Vec2() : Vec2(0, 0){};
        template <typename U>
        Vec2(const U &vec) : Vec2(vec[0], vec[1]) {}

        T &operator[](const size_t &dim) { return dim == 0 ? x : y; }
        T &operator()(const size_t &dim) { return dim == 0 ? x : y; }
        const T &operator[](const size_t &dim) const { return dim == 0 ? x : y; }
        const T &operator()(const size_t &dim) const { return dim == 0 ? x : y; }
        Vec2<T> &operator+=(const Vec2<T> &rhs)
        {
            for (size_t dim = 0; dim < DIM_; ++dim)
                this->operator[](dim) += rhs[dim];
            return *this;
        }
        friend Vec2<T> operator+(Vec2<T> lhs, const Vec2<T> &rhs)
        {
            lhs += rhs;
            return lhs;
        }
        Vec2<T> &operator+=(const T &rhs)
        {
            for (size_t dim = 0; dim < DIM_; ++dim)
                this->operator[](dim) += rhs;
            return *this;
        }
        friend Vec2<T> operator+(Vec2<T> lhs, const T &rhs)
        {
            lhs += rhs;
            return lhs;
        }
        Vec2<T> &operator+=(const T (&rhs)[2])
        {
            for (size_t dim = 0; dim < DIM_; ++dim)
                this->operator[](dim) += rhs[dim];
            return *this;
        }
        friend Vec2<T> operator+(Vec2<T> lhs, const T (&rhs)[2])
        {
            lhs += rhs;
            return lhs;
        }
        friend Vec2<T> operator+(Vec2<T> lhs, const std::array<T, 2> &rhs)
        {
            lhs += rhs;
            return lhs;
        }
        Vec2<T> &operator+=(const std::array<T, 2> &rhs)
        {
            for (size_t dim = 0; dim < DIM_; ++dim)
                this->operator[](dim) += rhs[dim];
            return *this;
        }
        Vec2<T> &operator-=(const Vec2<T> &rhs)
        {
            for (size_t dim = 0; dim < DIM_; ++dim)
                this->operator[](dim) -= rhs[dim];
            return *this;
        }
        friend Vec2<T> operator-(Vec2<T> lhs, const Vec2<T> &rhs)
        {
            lhs -= rhs;
            return lhs;
        }
        friend Vec2<T> operator-(Vec2<T> lhs)
        {
            lhs[0] = -lhs[0];
            lhs[1] = -lhs[1];
            return lhs;
        }
        Vec2<T> &operator-=(const T &rhs)
        {
            for (size_t dim = 0; dim < DIM_; ++dim)
                this->operator[](dim) -= rhs;
            return *this;
        }
        friend Vec2<T> operator-(Vec2<T> lhs, const T &rhs)
        {
            lhs -= rhs;
            return lhs;
        }
        Vec2<T> &operator-=(const T (&rhs)[2])
        {
            for (size_t dim = 0; dim < DIM_; ++dim)
                this->operator[](dim) -= rhs[dim];
            return *this;
        }
        friend Vec2<T> operator-(Vec2<T> lhs, const T (&rhs)[2])
        {
            lhs -= rhs;
            return lhs;
        }
        friend Vec2<T> operator-(Vec2<T> lhs, const std::array<T, 2> &rhs)
        {
            lhs -= rhs;
            return lhs;
        }
        Vec2<T> &operator-=(const std::array<T, 2> &rhs)
        {
            for (size_t dim = 0; dim < DIM_; ++dim)
                this->operator[](dim) -= rhs[dim];
            return *this;
        }
        Vec2<T> &operator*=(const Vec2<T> &rhs)
        {
            for (size_t dim = 0; dim < DIM_; ++dim)
                this->operator[](dim) *= rhs[dim];
            return *this;
        }
        friend Vec2<T> operator*(Vec2<T> lhs, const Vec2<T> &rhs)
        {
            lhs *= rhs;
            return lhs;
        }
        Vec2<T> &operator*=(const T &rhs)
        {
            for (size_t dim = 0; dim < DIM_; ++dim)
                this->operator[](dim) *= rhs;
            return *this;
        }
        friend Vec2<T> operator*(Vec2<T> lhs, const T &rhs)
        {
            lhs *= rhs;
            return lhs;
        }
        Vec2<T> &operator*=(const T (&rhs)[2])
        {
            for (size_t dim = 0; dim < DIM_; ++dim)
                this->operator[](dim) *= rhs[dim];
            return *this;
        }
        friend Vec2<T> operator*(Vec2<T> lhs, const T (&rhs)[2])
        {
            lhs *= rhs;
            return lhs;
        }
        friend Vec2<T> operator*(Vec2<T> lhs, const std::array<T, 2> &rhs)
        {
            lhs *= rhs;
            return lhs;
        }
        Vec2<T> &operator*=(const std::array<T, 2> &rhs)
        {
            for (size_t dim = 0; dim < DIM_; ++dim)
                this->operator[](dim) *= rhs[dim];
            return *this;
        }
        Vec2<T> &operator/=(const Vec2<T> &rhs)
        {
            for (size_t dim = 0; dim < DIM_; ++dim)
                this->operator[](dim) /= rhs[dim];
            return *this;
        }
        friend Vec2<T> operator/(Vec2<T> lhs, const Vec2<T> &rhs)
        {
            lhs /= rhs;
            return lhs;
        }
        Vec2<T> &operator/=(const T &rhs)
        {
            for (size_t dim = 0; dim < DIM_; ++dim)
                this->operator[](dim) /= rhs;
            return *this;
        }
        friend Vec2<T> operator/(Vec2<T> lhs, const T &rhs)
        {
            lhs /= rhs;
            return lhs;
        }
        Vec2<T> &operator/=(const T (&rhs)[2])
        {
            for (size_t dim = 0; dim < DIM_; ++dim)
                this->operator[](dim) /= rhs[dim];
            return *this;
        }
        friend Vec2<T> operator/(Vec2<T> lhs, const std::array<T, 2> &rhs)
        {
            lhs /= rhs;
            return lhs;
        }
        Vec2<T> &operator/=(const std::array<T, 2> &rhs)
        {
            for (size_t dim = 0; dim < DIM_; ++dim)
                this->operator[](dim) /= rhs[dim];
            return *this;
        }
        friend Vec2<T> operator/(Vec2<T> lhs, const T (&rhs)[2])
        {
            lhs /= rhs;
            return lhs;
        }
        friend Vec2<T> operator<(const Vec2<T> &lhs, const Vec2<T> &rhs)
        {
            Vec2<T> res;
            for (size_t dim = 0; dim < DIM_; ++dim)
                res[dim] = lhs[dim] < rhs[dim];
            return res;
        }
        friend Vec2<T> operator>(const Vec2<T> &lhs, const Vec2<T> &rhs)
        {
            Vec2<T> res;
            for (size_t dim = 0; dim < DIM_; ++dim)
                res[dim] = lhs[dim] > rhs[dim];
            return res;
        }
        friend Vec2<T> operator<=(const Vec2<T> &lhs, const Vec2<T> &rhs)
        {
            Vec2<T> res;
            for (size_t dim = 0; dim < DIM_; ++dim)
                res[dim] = lhs[dim] <= rhs[dim];
            return res;
        }
        friend Vec2<T> operator>=(const Vec2<T> &lhs, const Vec2<T> &rhs)
        {
            Vec2<T> res;
            for (size_t dim = 0; dim < DIM_; ++dim)
                res[dim] = lhs[dim] >= rhs[dim];
            return res;
        }
        friend bool operator==(const Vec2<T> &lhs, const Vec2<T> &rhs)
        {
            return lhs[0] == rhs[0] && lhs[1] == rhs[1];
        }
        friend bool operator!=(const Vec2<T> &lhs, const Vec2<T> &rhs)
        {
            return !(lhs == rhs);
        }
        friend bool operator==(const Vec2<T> &lhs, const T &rhs)
        {
            return lhs[0] == rhs && lhs[1] == rhs;
        }
        friend bool operator!=(const Vec2<T> &lhs, const T &rhs)
        {
            return !(lhs == rhs);
        }
        friend Vec2<T> operator!(const Vec2<T> &vec)
        {
            Vec2<T> res;
            for (size_t dim = 0; dim < DIM_; ++dim)
                res[dim] = !vec[dim];
            return res;
        }
        Vec2<T> &operator=(const T &rhs)
        {
            for (size_t dim = 0; dim < DIM_; ++dim)
                this->operator[](dim) = rhs;
            return *this;
        }

        Vec2<T> abs()
        {
            Vec2<T> res;
            for (size_t dim = 0; dim < DIM_; ++dim)
                res[dim] = std::abs(this->operator[](dim));
            return res;
        }

        Vec2<T> sgn()
        {
            Vec2<T> res;
            for (size_t dim = 0; dim < DIM_; ++dim)
                res[dim] = ee4308::sgn<T>(this->operator[](dim));
            return res;
        }

        Vec2<T> round()
        {
            Vec2<T> res;
            for (size_t dim = 0; dim < DIM_; ++dim)
                res[dim] = std::round(this->operator[](dim));
            return res;
        }

        Vec2<T> ceil()
        {
            Vec2<T> res;
            for (size_t dim = 0; dim < DIM_; ++dim)
                res[dim] = std::ceil(this->operator[](dim));
            return res;
        }

        Vec2<T> floor()
        {
            Vec2<T> res;
            for (size_t dim = 0; dim < DIM_; ++dim)
                res[dim] = std::floor(this->operator[](dim));
            return res;
        }

        T cross(const Vec2<T> &rhs) { return x * rhs.y - y * rhs.x; }

        T dot(const Vec2<T> &rhs) { return x * rhs.x + y * rhs.y; }

        T normsq() { return this->dot(*this); }

        double norm() { return std::sqrt(double(normsq())); }

        Eigen::Array2i toEigenArray2i() { return Eigen::Array2i(int(x), int(y)); }

        Eigen::Array2d toEigenArray2d() { return Eigen::Array2d(int(x), int(y)); }

        Eigen::Array2i toEigenVector2i() { return Eigen::Vector2i(int(x), int(y)); }

        Eigen::Array2d toEigenVector2d() { return Eigen::Vector2d(int(x), int(y)); }

        friend std::ostream &operator<<(std::ostream &out, Vec2<T> const &vec)
        {
            if (std::is_same<T, float>::value || std::is_same<T, double>::value)
                vec.stream(out, 7, 3);
            else
                vec.stream(out, 4, 0);
            return out;
        }
        std::ostream &stream(std::ostream &out, size_t width = 7, size_t precision = 3) const
        {
            if (std::is_same<T, float>::value || std::is_same<T, double>::value)
            {
                out << std::fixed;
                for (size_t dim = 0; dim < DIM_; ++dim)
                    out << std::setw(width) << std::setprecision(precision)
                        << this->operator[](dim) << (dim != 1 ? "," : "");
            }
            else
            {
                for (size_t dim = 0; dim < DIM_; ++dim)
                    out << std::setw(width)
                        << this->operator[](dim) << (dim != 1 ? "," : "");
            }
            return out;
        }
        std::string repr() const
        {
            std::stringstream ss;
            ss << *this;
            return ss.str();
        }
    };

}