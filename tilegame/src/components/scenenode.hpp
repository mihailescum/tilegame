#pragma once

#include <functional>

#include "scenegraphdata.hpp"

namespace tilegame::components
{
    struct SceneNode
    {
        // Use a pointer here because SceneGraphNode is not movable/copyable
        SceneGraphNode *node;

        const SceneGraphNode &operator()() const { return *node; }
        SceneGraphNode &operator()() { return *node; }
    };
} // namespace tilegame::components
