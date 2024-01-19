#pragma once

#include "engine.hpp"

#include "system.hpp"
#include "components/transform.hpp"
#include "components/sprite.hpp"
#include "components/tilelayer.hpp"
#include "components/particlepool.hpp"

namespace tilegame::systems
{
    class Render : public System
    {
    private:
        bool _needs_sorting;
        engine::graphics::SpriteBatch<engine::Texture2DContainer<2>> &_spritebatch;
        engine::Shader *_spritebatch_luminosity_shader;

        void draw_sprite(const components::Transform &transform, const components::Sprite &sprite) const;
        void draw_tilelayer(const components::Transform &transform, const components::TileLayer &tilelayer) const;
        void draw_particles(const components::ParticlePool &pool) const;

        void sort_renderables() const;
        void needs_sorting(entt::registry &, entt::entity) { _needs_sorting = true; }

        engine::Texture2DContainer<2> _rect_tex;
        engine::Texture2DContainer<2> _circle_tex;

    public:
        Render(tilegame::Scene &scene, entt::registry &registry, engine::graphics::SpriteBatch<engine::Texture2DContainer<2>> &spritebatch);

        void initialize();
        void load_content();
        void draw(const engine::GameTime &draw_time);
    };
} // namespace tilegame