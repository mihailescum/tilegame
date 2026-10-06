#include "interaction.hpp"

#include <limits>
#include <cmath>

#include <glm/glm.hpp>
#include <glm/gtx/norm.hpp>

#include "components/player.hpp"
#include "components/transform.hpp"
#include "components/facing.hpp"
#include "components/interactable.hpp"
#include "components/messagebox.hpp"

namespace tilegame::systems
{
    Interaction::Interaction(engine::Scene &scene, entt::registry &registry) : _enter_was_down(false), System(scene, registry)
    {
    }

    void Interaction::update(const engine::GameTime &update_time)
    {
        const auto &window = _scene.game().window();

        const bool enter_is_down = window.is_key_pressed(GLFW_KEY_ENTER);
        const bool enter_pressed = enter_is_down && !_enter_was_down;
        _enter_was_down = enter_is_down;

        const bool message_open = _registry.ctx().get<components::MessageBoxOpenState>().open;
        const bool should_search = enter_pressed && !message_open;

        if (should_search)
        {
            const auto players = _registry.view<const components::Player, const components::Transform>(entt::exclude<engine::Inactive>);
            const auto interactables = _registry.view<const components::Transform, const components::Interactable>(entt::exclude<engine::Inactive>);

            for (auto &&[player_entity, player, player_transform] : players.each())
            {
                entt::entity closest_entity = entt::null;
                float closest_distance2 = std::numeric_limits<float>::max();
                float cos_angle_player_interactable;

                for (auto &&[interactable_entity, interactable_transform, interactable_component] : interactables.each())
                {
                    const glm::vec2 interactable_to_player = player_transform.position - interactable_transform.position;
                    const float distance2 = glm::length2(interactable_to_player);
                    if (distance2 > closest_distance2 || distance2 > interactable_component.max_distance * interactable_component.max_distance)
                        continue;

                    // Skip the facing check right on top of the interactable, where the direction
                    // to it is undefined. Facing is already normalized, so only to_interactable needs it.
                    const auto *interactable_facing = _registry.try_get<components::Facing>(interactable_entity);
                    if (interactable_facing)
                    {
                        cos_angle_player_interactable = glm::dot(glm::normalize(interactable_to_player), (*interactable_facing)());
                        if (distance2 > 1e-10f && cos_angle_player_interactable < interactable_component.cos_max_angle)
                        {
                            continue;
                        }
                    }
                    else
                    {
                        // We cannot compute an angle, so we set some result which otherwise is impossible
                        cos_angle_player_interactable = std::numeric_limits<float>::quiet_NaN();
                    }

                    closest_entity = interactable_entity;
                    closest_distance2 = distance2;
                }

                if (closest_entity != entt::null)
                {
                    // target = player_entity, so a listener can scope itself to a specific
                    // player (e.g. local multiplayer) in addition to/instead of the source NPC.
                    raise_event<components::InteractEvent>(closest_entity, player_entity, std::sqrt(closest_distance2), cos_angle_player_interactable);
                }
            }
        }
    }
} // namespace tilegame::systems
