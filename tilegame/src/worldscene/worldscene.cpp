#include "worldscene.hpp"

#include "tilegame.hpp"

#include "components/renderable2d.hpp"
#include "components/ordering.hpp"
#include "components/scenenode.hpp"
#include "components/animation.hpp"

namespace tilegame::worldscene
{
    WorldScene::WorldScene(Tilegame &game)
        : tilegame::Scene(game),
          _system_render(*this, _registry, game.get_spritebatch()),
          _system_map(*this, _registry),
          _system_camera(*this, _registry),
          _system_player(*this, _registry),
          _system_movement(*this, _registry),
          _system_scenegraph(*this, _registry),
          _system_animation(*this, _registry)
    {
    }

    void WorldScene::initialize()
    {
        _system_scenegraph.initialize();
        _system_render.initialize();
        _system_animation.initialize();

        _system_player.initialize();
        _system_camera.initialize();
    }

    void WorldScene::load_content()
    {
        _system_map.load_content();
        /*_system_player.load_content();

        // TODO Remove this

        const auto soldier_entity = _registry.create();
        _registry.emplace<tilegame::components::Transform>(soldier_entity, glm::vec2(300, 300), glm::vec2(0.0));
        _registry.emplace<tilegame::components::Ordering>(soldier_entity, 3.0);
        _registry.emplace<tilegame::components::Movement>(soldier_entity, tilegame::components::Movement::None, 100.0);

        const tilegame::SceneGraphData soldier_scenedata(soldier_entity);
        tilegame::SceneGraphNode &soldier_scenenode = get_scene_graph_root().add_child(soldier_scenedata);
        _registry.emplace<tilegame::components::SceneNode>(soldier_entity, &soldier_scenenode);

        const engine::sprite::SpriteSheet *characters = _game.get_resource_manager().load_resource<engine::sprite::SpriteSheet>("characters", "content/characters/characters.tsx");
        const engine::Texture2D &characters_texture = characters->get_texture();

        const engine::sprite::Sprite &soldier_sprite = (*characters)["soldier"];

        _registry.emplace<tilegame::components::Renderable2D>(soldier_entity);
        const auto &soldier_animation_component = _registry.emplace<tilegame::components::Animation>(soldier_entity, 0.0, 0, soldier_sprite["right_walking"].frames);
        _registry.emplace<tilegame::components::Sprite>(soldier_entity, characters_texture, soldier_animation_component.frames[soldier_animation_component.current_frame_idx].source_rect);
    */
    }

    void WorldScene::unload_content()
    {
        _system_scenegraph.unload_content();
    }

    void WorldScene::update(const engine::GameTime &update_time)
    {
        _system_player.update(update_time);
        _system_movement.update(update_time);

        _system_animation.update(update_time);
        _system_scenegraph.update(update_time);

        _system_camera.update(update_time);
    }

    void WorldScene::draw(const engine::GameTime &draw_time)
    {
        _system_render.draw(draw_time);
    }
} // namespace tilegame::worldscene