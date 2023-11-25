#pragma once

namespace engine
{
    struct Color
    {
    public:
        constexpr Color(float r, float g, float b, float a) : r(r), g(g), b(b), a(a) {}
        constexpr Color() : Color(0.0, 0.0, 0.0, 0.0) {}

        float r;
        float g;
        float b;
        float a;

        static const Color WHITE;
        static const Color BLACK;
        static const Color CORNFLOWER_BLUE;
    };
} // namespace engine