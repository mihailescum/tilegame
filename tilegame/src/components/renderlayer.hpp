#pragma once

#include <vector>

#include "engine.hpp"

namespace tilegame::components
{
    struct RenderLayer
    {
        float z_index;
        std::vector<entt::entity> children;
    };
} // namespace tilegame
