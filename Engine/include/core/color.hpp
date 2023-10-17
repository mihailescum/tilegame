#pragma once

namespace engine
{
    struct Color
    {
    public:
        constexpr Color(double r, double g, double b, double a) : r(r), g(g), b(b), a(a) {}

        double r;
        double g;
        double b;
        double a;

        static const Color White;
        static const Color Black;
        static const Color CornflowerBlue;
    };
} // namespace engine