#include "core/color.hpp"

namespace engine
{
    const Color Color::WHITE(1.0f, 1.0f, 1.0f, 1.0f);
    const Color Color::BLACK(0.0f, 0.0f, 0.0f, 1.0f);
    const Color Color::CORNFLOWER_BLUE(0.39f, 0.58f, 0.93f, 1.0f);

    Color engine::Color::lerp(const Color &a, const Color &b, float amount)
    {
        return Color((1.0f - amount) * static_cast<glm::vec4>(a) + (amount) * static_cast<glm::vec4>(b));
    }
}