#pragma once

#include <vector>

#include "entt.hpp"

namespace tilegame
{
    struct ChildrenComponent
    {
        std::vector<entt::entity> children;
    };
} // namespace tilegame
