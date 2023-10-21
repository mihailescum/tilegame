#pragma once

#include <vector>

#include "entt.hpp"

namespace tilegame::components
{
    struct Children
    {
        std::vector<entt::entity> children;
    };
} // namespace tilegame
