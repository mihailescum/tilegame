#include "collider.hpp"

#include <stdexcept>

namespace tilegame::components
{
    engine::ShapeVariant Collider::make_shape(const sol::table &descriptor)
    {
        const std::string kind = descriptor["kind"];
        const glm::vec2 position = descriptor["position"];

        if (kind == "rectangle")
        {
            const glm::vec2 dimensions = descriptor["dimensions"];
            return engine::Rectangle(position, dimensions);
        }
        else if (kind == "circle")
        {
            const float radius = descriptor["radius"];
            return engine::Circle(position, radius);
        }
        else if (kind == "point")
        {
            return engine::Point(position);
        }
        else
        {
            throw std::runtime_error("Unknown collider shape kind");
        }
    }
} // namespace tilegame::components
