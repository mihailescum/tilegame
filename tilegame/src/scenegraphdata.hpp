#pragma once

#include "entt/entt.hpp"

#include "engine.hpp"
#include "components/transform.hpp"

namespace tilegame
{
    /**
     * @brief Payload attached to each engine scene-graph node, tying it to its entt entity.
     *
     * Lets code holding a SceneGraphNode look up (or, via null, note the
     * absence of) the entt::registry entity it represents.
     */
    struct SceneGraphData
    {
        entt::entity entity;

        SceneGraphData() : SceneGraphData(static_cast<entt::entity>(entt::null)) {}
        SceneGraphData(entt::entity entity) : entity(entity) {}
    };

    // tilegame's concrete instantiations of the engine's generic scene-graph/scene
    // templates, parameterized over SceneGraphData.
    typedef engine::SceneGraphNode<SceneGraphData> SceneGraphNode;
    typedef engine::Scene<SceneGraphData> Scene;
} // namespace tilegame
