#pragma once

#include <glm/glm.hpp>

namespace engine
{
    /**
     * @brief RGBA color value, stored internally as a glm::vec4 with components in the [0, 1] range.
     */
    struct Color
    {
    public:
        constexpr Color() : Color(0.0, 0.0, 0.0, 0.0) {}
        constexpr Color(float r, float g, float b, float a) : _native(r, g, b, a) {}
        constexpr Color(glm::vec4 color) : _native(color) {}

        float r() const { return _native.x; }
        void r(float r) { _native.x = r; }

        float g() const { return _native.y; }
        void g(float g) { _native.y = g; }

        float b() const { return _native.z; }
        void b(float b) { _native.z = b; }

        float a() const { return _native.w; }
        void a(float a) { _native.w = a; }

        operator glm::vec4() const { return _native; }

        /// Linearly interpolates between two colors; @p amount is clamped to [0, 1] usage but not clamped internally.
        static Color lerp(const Color &a, const Color &b, float amount);

        static const Color WHITE;
        static const Color BLACK;
        static const Color CORNFLOWER_BLUE;

    private:
        glm::vec4 _native;
    };
} // namespace engine