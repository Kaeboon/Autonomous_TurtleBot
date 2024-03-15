#include <cmath>

#pragma once
namespace ee4308
{
    double THRES = 1e-8;
    

    template <typename T>
    T sgn(const T &value) { return (T(0) < value) - (value < T(0)); }
    
    // returns true if val1 - THRES is gt val2
    bool approxGt(const double &val1, const double &val2) { return val1 - THRES > val2; }

    // returns true if val1 + THRES is gt val2
    bool approxGe(const double &val1, const double &val2) { return val1 + THRES > val2; }

    // returns true if abs(val1 - val2) < THRES*2
    bool approxEq(const double &val1, const double &val2) { return std::abs(val1 - val2) < THRES; }


    double limit_angle(double angle)
    {
        double result = fmod(angle + M_PI, M_PI*2); // fmod rounds remainders to zero. we want remainders to be +ve like mod() in matlab and % in python
        return result >= 0 ? result - M_PI : result + M_PI;
    }
}
