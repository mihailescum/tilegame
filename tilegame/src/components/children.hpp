#pragma once

#include <vector>

#include <entt/entity/registry.hpp>

namespace tilegame::components
{
    struct Children
    {
        std::vector<entt::entity> children;
    };
} // namespace tilegame
