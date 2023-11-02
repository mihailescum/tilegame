#pragma once

#include <functional>

#include "scenegraphdata.hpp"

namespace tilegame::components
{
    struct SceneNode
    {
        // Use a pointer here because SceneGraphNode is not movable/copyable
        SceneGraphNode *node;
    };
} // namespace tilegame::components
