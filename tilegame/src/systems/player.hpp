#pragma once

#include <string>

#include "engine.hpp"

#include "components/movement.hpp"

namespace tilegame::systems
{
    /**
     * @brief Spawns and drives the keyboard-controlled player entity.
     *
     * Creates the player 1 entity (Player, Transform, Speed, Direction,
     * Movement, Animation, Sprite, Collider, plus Facing/components::SpriteOrientation via
     * components::SpriteOrientation::make_orientable_if_directional() - see
     * components/spriteorientation.hpp - since the "man" sprite class defines all four
     * directions) on load_content, and each frame reads keyboard input for entities with a
     * Player + Speed component and writes the resulting Direction, which MovementController
     * later turns into velocity, and which systems::Facing in turn uses to keep the player's
     * Facing pointing the way it last moved. Also registers, on `_player1_entity` in load_content(),
     * EventListener<StopPlayerInputEvent>/EventListener<ResumePlayerInputEvent> that toggle
     * input the instant Lua's `_stop_player_input`/`_resume_player_input` raises them via the
     * inherited System::raise(), for whichever player entity's id matches the event's
     * player_id.
     */
    class Player : public engine::System
    {
    private:
        entt::entity _player1_entity;
        bool _player1_input_active;
        // Reads the arrow keys for player 1 and returns a normalized direction vector (zero
        // if no keys are pressed).
        glm::vec2 handle_input_1();

    public:
        Player(engine::Scene &scene, entt::registry &registry);

        void initialize();
        void load_content();
        void update(const engine::GameTime &update_time);
    };
} // namespace tilegame