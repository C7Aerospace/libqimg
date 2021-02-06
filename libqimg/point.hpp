
//  Copyright 2021 Isoheptane
//  Filename    : point.hpp
//  Purpose     : Type of 2-Dimensional PointF
//  License     : MIT License

#ifndef _LIBQIMG_POINTF_HPP_
#define _LIBQIMG_POINTF_HPP_

#include <cmath>

#include "libqimg_math.hpp"

namespace libqimg {

    // 2-Dimensional Point
    struct PointF {
        // Horiziontal Coordinate of PointF
        float x;
        // Vertical Coordinate of PointF
        float y;

        PointF() {}
        PointF(float x, float y):x(x), y(y) {}
        /* Vector-Like operators */
        inline PointF operator+(const PointF& b) { return (PointF){x + b.x, y + b.y}; }
        inline PointF operator-(const PointF& b) { return (PointF){x - b.x, y - b.y}; }
        inline PointF operator*(float b) { return (PointF){x * b, y * b}; }
        inline PointF operator/(float b) { return (PointF){x / b, y / b}; }
        /* Vector-Like self operators */
        inline PointF operator+=(const PointF& b) { return (*this) = (PointF){x + b.x, y + b.y}; }
        inline PointF operator-=(const PointF& b) { return (*this) = (PointF){x - b.x, y - b.y}; }
        inline PointF operator*=(float b) { return (*this) = (PointF){x * b, y * b}; }
        inline PointF operator/=(float b) { return (*this) = (PointF){x / b, y / b}; }
        /* Functions */
        // Euclidian distance to origin
        inline float distanceToOrigin() const { return sqrtf(x * x + y * y); }
        // Euclidian distance to the target point
        inline float distanceTo(const PointF& to) const { return ((PointF){x - to.x, y - to.y}).distanceToOrigin(); }
        /* Static functions */
        // Euclidian distance between two points
        static inline float distance(const PointF& a, const PointF& b) {
            return ((PointF){a.x - b.x, b.y - b.y}).distanceToOrigin(); 
        }
        static inline PointF lerp(const PointF& a, const PointF& b, float t) {
            return PointF(math::lerp(a.x, b.x, t), math::lerp(a.y, b.y, t));
        }
        static inline PointF lerp(const PointF& a, const PointF& b, const PointF& t) {
            return PointF(math::lerp(a.x, b.x, t.x), math::lerp(a.y, b.y, t.y));
        }
        static inline PointF reverseLerp(const PointF& a, const PointF& b, const PointF& value) {
            return PointF(
                math::reverseLerp(a.x, b.x, value.x), 
                math::reverseLerp(a.y, b.y, value.y));
        }
    };
    // 2-Dimensional Point
    struct Point {
        // Horiziontal Coordinate of PointF
        int x;
        // Vertical Coordinate of PointF
        int y;

        Point() {}
        Point(int x, int y):x(x), y(y) {}
        Point(PointF pf):x(static_cast<int>(round(pf.x))), y(static_cast<int>(round(pf.y))) {}
        /* Vector-Like operators */
        inline Point operator+(const Point& b) { return (Point){x + b.x, y + b.y}; }
        inline Point operator-(const Point& b) { return (Point){x - b.x, y - b.y}; }
        inline Point operator*(int b) { return (Point){x * b, y * b}; }
        /* Vector-Like self operators */
        inline Point operator+=(const Point& b) { return (*this) = (Point){x + b.x, y + b.y}; }
        inline Point operator-=(const Point& b) { return (*this) = (Point){x - b.x, y - b.y}; }
        inline Point operator*=(int b) { return (*this) = (Point){x * b, y * b}; }

    };

}

#endif