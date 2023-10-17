#pragma once

#include "scene/entity.hpp"

namespace engine
{
    struct ChildComponent
    {
        Entity child;

        ChildComponent() = default;
        ChildComponent(const ChildComponent &) = default;
        ChildComponent(const Entity &child)
            : child(child) {}

        operator Entity &() { return this->child; }
        operator const Entity &() const { return this->child; }
    };
} // namespace engine