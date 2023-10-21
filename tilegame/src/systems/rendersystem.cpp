#include "rendersystem.hpp"

#include "components/renderable2d.hpp"
#include "components/worldtransform.hpp"

namespace tilegame::systems
{
    RenderSystem::RenderSystem(engine::Scene &scene, entt::registry &registry, engine::SpriteBatch &spritebatch) : System(scene, registry), _spritebatch(spritebatch)
    {
    }

    void RenderSystem::draw()
    {
        auto view = _registry.view<const tilegame::components::WorldTransform, const tilegame::components::Renderable2D>();

        _spritebatch.begin(false);
        for (const auto entity : view)
        {
            auto &&[world, render] = view.get<const tilegame::components::WorldTransform, const tilegame::components::Renderable2D>(entity);
            engine::Rectangle dest_rect(world.position.x, world.position.y, render.texture->get_width(), render.texture->get_height());
            _spritebatch.draw(*render.texture, dest_rect, engine::Color::WHITE);
        }
        _spritebatch.end();
    }
}