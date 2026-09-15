#pragma once

#include <functional>

#include "scenegraphdata.hpp"

namespace tilegame::components
{
    /**
     * @brief Links an entity to its node in the engine's hierarchical scene graph (see
     * scenegraphdata.hpp), letting the entity be positioned relative to a parent node. Currently
     * unused by any active system (construction sites are commented out throughout the codebase).
     */
    struct SceneNode
    {
        // Use a pointer here because SceneGraphNode is not movable/copyable
        SceneGraphNode *node;

        const SceneGraphNode &operator()() const { return *node; }
        SceneGraphNode &operator()() { return *node; }
    };
} // namespace tilegame::components
