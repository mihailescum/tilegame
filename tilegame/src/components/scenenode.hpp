#pragma once

#include <functional>

#include "engine.hpp"

namespace tilegame::components
{
    struct SceneNode
    {
        std::reference_wrapper<engine::SceneGraphNode<int>> node;
    };
} // namespace tilegame::components
