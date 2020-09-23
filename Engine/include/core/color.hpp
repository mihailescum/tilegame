#ifndef __ENGINE_COLOR_H__
#define __ENGINE_COLOR_H__

namespace engine
{
    struct Color
    {
    public:
        Color(const float r, const float g, const float b, const float a) : r(r), g(g), b(b), a(a) {}
        
        float r;
        float g;
        float b;
        float a;

        static const Color White;
        static const Color Black;
        static const Color CornflowerBlue;
    };
} // namespace engine

#endif