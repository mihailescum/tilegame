#pragma once

#include "engine.hpp"

#include "system.hpp"
#include "components/transform.hpp"
#include "components/sprite.hpp"
#include "components/tilelayer.hpp"
#include "components/particlepool.hpp"
#include "components/messagebox.hpp"

namespace tilegame::systems
{
    /**
     * @brief Draws all renderable entities through the game's single camera.
     *
     * Operates on entities with Transform + Renderable2D, dispatching each to
     * the right draw path based on whether it also has a Sprite, TileLayer,
     * or ParticlePool component, batched into the SpriteBatch. The camera
     * itself is looked up from the registry context (components::CAMERA_ENTITY_ID,
     * set by systems::Camera) rather than iterated as a view, since there is
     * only ever one. Draw order follows the Ordering component, re-sorted
     * lazily whenever it changes. Also draws debug overlays for
     * Collider/TileLayer collision shapes. Owns the PostProcessor (day/night
     * tint, then a two-pass Gaussian blur of the per-sprite luminosity output
     * - gated by components::NIGHT_AMOUNT_ID, set by systems::Daytime, so
     * light sources only bloom once it's actually dark - additively blended
     * back onto the tinted scene), which wraps that whole scene; the dialog
     * box, read from the registry's components::MessageBoxState ctx() value
     * (owned by systems::MessageBox), is drawn last, screen-space, straight
     * onto the default framebuffer, so it's unaffected by both the camera
     * and the post-processing effects.
     */
    class Render : public System
    {
    private:
        bool _needs_sorting;
        // Owned here rather than by Tilegame/Game: nothing outside of Render draws anything, so
        // there's no reason for the SpriteBatch(es)/PostProcessor to live any higher up (unless/until
        // the game grows more scenes than just WorldScene and switching between them needs to reuse
        // the same GL VAO/VBO/shader/FBOs instead of recreating them per scene).
        engine::graphics::SpriteBatch<engine::Texture2DContainer<2>> _spritebatch;
        // Separate batch dedicated to text: draw_text() only exists for SpriteBatch<Texture2D>,
        // since it binds a SpriteFont's texture directly rather than through the main batch's
        // Texture2DContainer<2>.
        engine::graphics::SpriteBatch<engine::Texture2D> _text_spritebatch;
        engine::Shader *_spritebatch_luminosity_shader;
        // Day/night tint + blend effect chain; wraps everything drawn in draw() except the
        // dialog box, which is drawn after apply_effects().
        engine::graphics::PostProcessor _postprocessor;

        void draw_sprite(const components::Transform &transform, const components::Sprite &sprite);
        // `visible_bounds` is the camera's world-space view rectangle (components::Camera::visible_bounds,
        // recomputed each frame by systems::Camera); tiles/particles whose destination rect doesn't
        // intersect it are skipped.
        void draw_tilelayer(const components::Transform &transform, const components::TileLayer &tilelayer, const engine::Rectangle &visible_bounds);
        void draw_particles(const components::ParticlePool &pool, const engine::Rectangle &visible_bounds);
        // Draws the screen-space dialog box (background + up to messagebox_layout::VISIBLE_LINES
        // lines of glyphs) for a non-empty components::MessageBoxState; called with its own
        // spritebatch begin/end so it isn't affected by any camera transform.
        void draw_message_box(const components::MessageBoxState &state);

        void sort_renderables();
        // Registered on construct/update/destroy of Ordering components so draw order is
        // only re-sorted (lazily, on the next draw()) when it may actually have changed.
        void needs_sorting(entt::registry &, entt::entity) { _needs_sorting = true; }

        engine::Texture2DContainer<2> _rect_tex;
        engine::Texture2DContainer<2> _circle_tex;
        const engine::graphics::SpriteFont *_font;

    public:
        Render(tilegame::Scene &scene, entt::registry &registry);

        void initialize();
        void load_content();
        void draw(const engine::GameTime &draw_time);
    };
} // namespace tilegame