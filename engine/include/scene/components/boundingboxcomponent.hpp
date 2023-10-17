#pragma once

#include "entt.hpp"

#include "core/rectangle.hpp"

namespace engine
{
    struct BoundingBoxComponent
    {
    public:
        Rectangle boundingBox;

        BoundingBoxComponent() = default;
        BoundingBoxComponent(const BoundingBoxComponent &component) = default;
        BoundingBoxComponent(const Rectangle &boundingBox)
            : boundingBox(boundingBox) {}

        operator const Rectangle &() const { return this->boundingBox; }
    };
} // namespace engine