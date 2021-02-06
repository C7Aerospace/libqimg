
//  Copyright 2021 Isoheptane
//  Filename    : color.hpp
//  Purpose     : Support Basic RGB Colors
//  License     : MIT License

#ifndef _LIBQIMG_COLOR_HPP_
#define _LIBQIMG_COLOR_HPP_

#include "libqimg_math.hpp"

namespace libqimg::TileMode {

    // Basic float RGBA Colors
    struct Color {

        float r = 0.0f;
        float g = 0.0f;
        float b = 0.0f;
        float a = 1.0f;

        Color() {}
        Color(float brightness):r(brightness), g(brightness), b(brightness) {};
        Color(float r, float g, float b):r(r), g(g), b(b) {};
        Color(float r, float g, float b, float a):r(r), g(g), b(b), a(a) {};
        /* Vector-Like operators */
        inline Color operator+(const Color& x) { return Color(r + x.r, g + x.g, b + x.b, a + x.a); }
        inline Color operator-(const Color& x) { return Color(r - x.r, g - x.g, b - x.b, a - x.a); }
        inline Color operator*(float x) { return Color(r * x, g * x, b * x, a * x); }
        inline Color operator/(float x) { return Color(r / x, g / x, b / x, a / x); }
        /* Vector-Like self operators */
        inline Color operator+(const Color& x) { return Color(r + x.r, g + x.g, b + x.b, a + x.a); }
        inline Color operator-(const Color& x) { return Color(r - x.r, g - x.g, b - x.b, a - x.a); }
        inline Color operator*(float x) { return Color(r * x, g * x, b * x, a * x); }
        inline Color operator/(float x) { return Color(r / x, g / x, b / x, a / x); }

        // Lightness = (max(R, G, B) + min(R, G, B)) / 2
        inline float lightness() { return (math::max(r, math::max(g, b)) + math::min(r, math::min(g, b))) / 2.0f; }
        // Return the average value of these channels
        inline float average() { return (r + g + b) / 3.0f; }
        // Luminosity = 0.21R + 0.72G + 0.07B
        inline float luminosity() { return r * 0.21f + g * 0.72f + b * 0.07f; }
        // Gray = 0.299R + 0.587G + 0.114B
        inline float gray() { return r * 0.299f + g * 0.587f + b * 0.114f; }

    };

}

#endif