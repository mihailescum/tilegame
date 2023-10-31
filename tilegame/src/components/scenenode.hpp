#pragma once

#include <functional>

#include "scenegraphdata.hpp"

namespace tilegame::components
{
    struct SceneNode
    {
        std::reference_wrapper<tilegame::SceneGraphNode> node;
    };
} // namespace tilegame::components
