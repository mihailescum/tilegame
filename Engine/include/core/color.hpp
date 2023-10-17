#pragma once

namespace engine
{
    struct Color
    {
    public:
        Color(const double r, const double g, const double b, const double a) : r(r), g(g), b(b), a(a) {}
        
        double r;
        double g;
        double b;
        double a;

        static const Color White;
        static const Color Black;
        static const Color CornflowerBlue;
    };
} // namespace engine