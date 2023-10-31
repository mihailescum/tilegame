#include "scenegraphsystem.hpp"

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
        _registry.emplace<tilegame::components::Transform>(root_entity, glm::vec2(0.0), glm::vec2(0.0));

        tilegame::SceneGraphData root_scenedata(root_entity);
        _scene.get_scene_graph_root().set_data(root_scenedata);
        _registry.emplace<tilegame::components::SceneNode>(root_entity, &_scene.get_scene_graph_root());

        _transformation_observer.connect(
            _registry,
            entt::collector.update<tilegame::components::Transform>().where<tilegame::components::SceneNode>());
    }

    void SceneGraphSystem::unload_content()
    {
        _transformation_observer.disconnect();
    }

    void SceneGraphSystem::update(const engine::GameTime &update_time)
    {
        if (_is_first_update)
        {
            auto &root_node = _scene.get_scene_graph_root();
            update_node(root_node);
            _is_first_update = false;
        }

        for (const auto entity : _transformation_observer)
        {
            auto &scene_node_component = _registry.get<tilegame::components::SceneNode>(entity);
            update_node(*scene_node_component.node);
        }

        _transformation_observer.clear();
    }

    void SceneGraphSystem::update_node(tilegame::SceneGraphNode &node)
    {
        const auto entity = node.get_data().entity;
        const auto parent = node.get_parent();

        auto &transform_component = _registry.get<tilegame::components::Transform>(entity);
        if (parent)
        {
            const auto parent_entity = parent->get_data().entity;
            auto parent_transform = _registry.try_get<tilegame::components::Transform>(parent_entity);

            if (parent_transform)
            {
                transform_component.position_global = transform_component.position_local + parent_transform->position_global;
            }
            else
            {
                transform_component.position_global = transform_component.position_local;
            }
        }
        else
        {
            transform_component.position_global = transform_component.position_local;
        }

        auto &children = node.get_children();
        for (int i = 0; i < children.size(); ++i)
        {
            auto child = children[i].get();
            update_node(*child);
        }
    }
} // namespace tilegame::systems
