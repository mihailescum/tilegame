#pragma once

#include "engine.hpp"

#include "system.hpp"
#include "components/transform.hpp"
#include "components/sprite.hpp"
#include "components/tilelayer.hpp"
#include "components/particlepool.hpp"

namespace tilegame::systems
{
    class RenderSystem : public System
    {
    private:
        bool _needs_sorting;
        engine::SpriteBatch &_spritebatch;

        void draw_sprite(const components::Transform &transform, const components::Sprite &sprite) const;
        void draw_tilelayer(const components::Transform &transform, const components::TileLayer &tile_layer) const;
        void draw_particles(const components::ParticlePool &pool) const;

        void sort_renderables() const;
        void needs_sorting() { _needs_sorting = true; }

    public:
        RenderSystem(tilegame::Scene &scene, entt::registry &registry, engine::SpriteBatch &spritebatch);

        void initialize();
        void draw(const engine::GameTime &draw_time);
    };
} // namespace tilegame