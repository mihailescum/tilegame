#include "scenegraph.hpp"

#include "components/transform.hpp"
#include "components/scenenode.hpp"

namespace tilegame::systems
{
    SceneGraphSystem::SceneGraphSystem(tilegame::Scene &scene, entt::registry &registry) : System(scene, registry)
    {
    }

    void SceneGraphSystem::initialize()
    {
        const auto root_entity = _registry.create();
        _registry.emplace<components::Transform>(root_entity, glm::vec2(0.0), glm::vec2(0.0));

        tilegame::SceneGraphData root_scenedata(root_entity);
        _scene.scene_graph_root().data(root_scenedata);
        _registry.emplace<components::SceneNode>(root_entity, &_scene.scene_graph_root());

        _transformation_observer.connect(
            _registry,
            entt::collector.update<components::Transform>().where<components::SceneNode>());
    }

    void SceneGraphSystem::unload_content()
    {
        _transformation_observer.disconnect();
    }

    void SceneGraphSystem::update(const engine::GameTime &update_time)
    {
        if (_is_first_update)
        {
            const auto &root_node = _scene.scene_graph_root();
            update_node_transform(root_node);
            _is_first_update = false;
        }

        for (const auto entity : _transformation_observer)
        {
            const auto &scene_node_component = _registry.get<components::SceneNode>(entity);
            update_node_transform(scene_node_component());
        }

        _transformation_observer.clear();
    }

    void SceneGraphSystem::update_node_transform(const tilegame::SceneGraphNode &node)
    {
        const auto entity = node.data().entity;
        const auto parent = node.parent();

        auto &transform_component = _registry.get<components::Transform>(entity);

        entt::entity parent_entity = entt::null;
        if (parent)
        {
            parent_entity = parent->data().entity;
        }

        const auto parent_transform = _registry.try_get<components::Transform>(parent_entity);

        if (parent_transform)
        {
            transform_component.position_global = transform_component.position_local + parent_transform->position_global;
        }
        else
        {
            transform_component.position_global = transform_component.position_local;
        }

        const auto &children = node.children();
        for (const auto &child : children)
        {
            update_node_transform(*child);
        }
    }
} // namespace tilegame::systems
