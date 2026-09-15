#pragma once

#include <string>

#include "engine.hpp"

#include "system.hpp"
#include "components/movement.hpp"

namespace tilegame::systems
{
    /**
     * @brief Spawns and drives the keyboard-controlled player entity.
     *
     * Creates the player 1 entity (Player, Transform, Speed, Direction,
     * Movement, Animation, Sprite, Collider) on load_content, and each frame
     * reads keyboard input for entities with a Player + Speed component and
     * writes the resulting Direction, which MovementController later turns
     * into velocity.
     */
    class Player : public System
    {
    private:
        entt::entity _player1_entity;
        // Reads the arrow keys for player 1 and returns a normalized direction vector (zero
        // if no keys are pressed).
        glm::vec2 handle_input_1();

    public:
        Player(tilegame::Scene &scene, entt::registry &registry);

        void initialize();
        void load_content();
        void update(const engine::GameTime &update_time);
    };
} // namespace tilegame