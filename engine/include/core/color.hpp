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

        static const Color WHITE;
        static const Color BLACK;
        static const Color CORNFLOWER_BLUE;
    };
} // namespace engine