#pragma once

#include <entt/entity/registry.hpp>

#include "engine.hpp"
#include "components/transform.hpp"

namespace tilegame
{
    struct SceneGraphData
    {
        entt::entity entity;
        tilegame::components::Transform *transform_component;

        SceneGraphData() : SceneGraphData(entt::null, nullptr) {}
        SceneGraphData(entt::entity entity, tilegame::components::Transform *transform_component)
            : entity(entity), transform_component(transform_component)
        {
        }
    };

    typedef engine::SceneGraphNode<SceneGraphData> SceneGraphNode;
    typedef engine::Scene<SceneGraphData> Scene;
} // namespace tilegame
