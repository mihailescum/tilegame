#pragma once

#include "engine.hpp"

#include "system.hpp"
#include "components/transform.hpp"
#include "components/sprite.hpp"
#include "components/tilelayer.hpp"
#include "components/particlepool.hpp"
#include "components/depth.hpp"

namespace tilegame::systems
{
    /**
     * @brief Draws all renderable entities through the game's single camera.
     *
     * Two passes, mirroring an ordinary opaque/transparent 3D split (see components::Depth):
     * opaque (every tile and sprite - alpha blending off, the GPU depth test on and writing, so
     * draw order within the pass doesn't matter - each draw's own Depth.z wins or loses per pixel
     * against whatever else was drawn there, no CPU sort anywhere) then transparent (ParticlePool,
     * e.g. weather - depth-tested against what the opaque pass left behind but not depth-writing,
     * real alpha blending, one Z per emitter). The camera itself is looked up from the registry
     * context (components::CAMERA_ENTITY_ID, set by systems::Camera)
     * rather than iterated as a view, since there is only ever one. Also draws debug overlays for
     * Collider/TileLayer collision shapes (depth testing off, always visible). Owns the
     * PostProcessor (day/night tint, then a two-pass Gaussian blur of the per-sprite luminosity
     * output - gated by components::NIGHT_AMOUNT_ID, set by systems::Daytime, so light sources
     * only bloom once it's actually dark - additively blended back onto the tinted scene), which
     * wraps that whole scene. The dialog/options message box is no longer drawn here - it's
     * scenes::UIScene, pushed on top of this scene and drawn after it (see SceneManager::draw()),
     * unaffected by both the camera and this system's post-processing effects.
     */
    class Render : public System
    {
    private:
        // How much of a components::Depth bucket's headroom one world-space unit of distance
        // from components::DepthOrigin.y consumes. Chosen so that even the worst case (an entity
        // up to ~2x components::DepthOrigin::CHUNK_SIZE away from the origin, which can happen
        // right at a chunk boundary) stays comfortably within one bucket's gap to the next -
        // 2 * 65536 * 0.000002 =~ 0.26, well inside e.g. the 0.49 gap between the dynamic bucket
        // (0.5) and weather's (0.99).
        static constexpr float DEPTH_FINE_SCALE = 0.000002f;

        // Owned here rather than by Tilegame/Game: nothing outside of Render draws anything, so
        // there's no reason for the SpriteBatch(es)/PostProcessor to live any higher up (unless/until
        // the game grows more scenes than just WorldScene and switching between them needs to reuse
        // the same GL VAO/VBO/shader/FBOs instead of recreating them per scene).
        engine::graphics::SpriteBatch<engine::Texture2DContainer<2>> _spritebatch;
        engine::Shader *_spritebatch_luminosity_shader;
        // Day/night tint + blend effect chain; wraps everything drawn in draw() except the
        // dialog box, which is drawn after apply_effects().
        engine::graphics::PostProcessor _postprocessor;

        // `depth_origin_y` is this frame's components::DepthOrigin.y (read once in draw() off the
        // camera entity) - see compute_z().
        void draw_sprite(const components::Transform &transform, const components::Sprite &sprite, const components::Depth &depth, float depth_origin_y);
        // `visible_bounds` is the camera's world-space view rectangle (components::Camera::visible_bounds,
        // recomputed each frame by systems::Camera); tiles/particles whose destination rect doesn't
        // intersect it are skipped. Each cell draws at its own baked TileData::depth/reference_y
        // (see those fields) - the layer entity itself carries no components::Depth of its own.
        void draw_tilelayer(const components::Transform &transform, const components::TileLayer &tilelayer, const engine::Rectangle &visible_bounds, float depth_origin_y);
        // `depth`/`emitter_transform` (the pool entity's own, not any one particle's) are used
        // once to compute a single Z for the whole pool - "a single depth value per emitter",
        // not one per particle.
        void draw_particles(const components::Transform &emitter_transform, const components::ParticlePool &pool, const components::Depth &depth, const engine::Rectangle &visible_bounds, float depth_origin_y);
        // The Z a draw call is actually issued with: `depth_base` (a components::Depth.z or
        // TileData.depth - which bucket this belongs in) plus a small contribution from
        // `world_y`'s distance to this frame's components::DepthOrigin.y, which keeps that
        // distance - and the float precision it needs - small no matter how far into the map
        // `world_y` actually is. See components::DepthOrigin for the full reasoning.
        float compute_z(float depth_base, float world_y, float depth_origin_y) const;

        engine::Texture2DContainer<2> _rect_tex;
        engine::Texture2DContainer<2> _circle_tex;

    public:
        Render(tilegame::Scene &scene, entt::registry &registry);

        void initialize();
        void load_content();
        void draw(const engine::GameTime &draw_time);
    };
} // namespace tilegame
