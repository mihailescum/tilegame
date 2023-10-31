#pragma once

#include <entt/entity/registry.hpp>

#include "engine.hpp"
#include "components/transform.hpp"

namespace tilegame
{
    struct SceneGraphData
    {
        entt::entity entity;

        SceneGraphData() : SceneGraphData(static_cast<entt::entity>(entt::null)) {}
        SceneGraphData(entt::entity entity) : entity(entity) {}
    };

    typedef engine::SceneGraphNode<SceneGraphData> SceneGraphNode;
    typedef engine::Scene<SceneGraphData> Scene;
} // namespace tilegame
