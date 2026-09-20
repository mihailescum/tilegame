#include "interaction.hpp"

#include <glm/glm.hpp>
#include <glm/gtx/norm.hpp>

#include "components/player.hpp"
#include "components/transform.hpp"
#include "components/facing.hpp"
#include "components/interactable.hpp"
#include "components/messagebox.hpp"
#include "components/inactive.hpp"

namespace tilegame::systems
{
    namespace
    {
        // Hardcoded for now; revisit (e.g. per-Interactable range, configurable from Lua) once a
        // real use case needs it to vary.
        constexpr float kInteractionDistance = 48.0f;
        // cos() of the half-angle of the cone in front of the player that still counts as
        // "facing" the interactable; ~60 degrees to either side.
        constexpr float kInteractionFacingCosThreshold = 0.5f;
    } // namespace

    Interaction::Interaction(tilegame::Scene &scene, entt::registry &registry) : _enter_was_down(false), _message_was_open_previous_frame(false), System(scene, registry)
    {
    }

    void Interaction::update(const engine::GameTime &update_time)
    {
        const auto &window = _scene.game().window();

        const bool enter_is_down = window.is_key_pressed(GLFW_KEY_ENTER);
        const bool enter_pressed = enter_is_down && !_enter_was_down;
        _enter_was_down = enter_is_down;

        // Read before acting, so a message MessageBox already closed via this same Enter press
        // (earlier in this same frame) still counts as "was open" for the hysteresis check below.
        const bool message_was_open = !_registry.ctx().get<components::MessageBoxState>().lines.empty();
        const bool should_search = enter_pressed && !message_was_open && !_message_was_open_previous_frame;

        if (should_search)
        {
            const auto players = _registry.view<const components::Player, const components::Transform, const components::Facing>(entt::exclude<components::Inactive>);
            const auto interactables = _registry.view<const components::Transform, const components::Interactable>(entt::exclude<components::Inactive>);

            for (auto &&[player_entity, player, player_transform, facing] : players.each())
            {
                entt::entity closest_entity = entt::null;
                float closest_distance2 = kInteractionDistance * kInteractionDistance;

                for (auto &&[interactable_entity, interactable_transform] : interactables.each())
                {
                    const glm::vec2 to_interactable = interactable_transform.position - player_transform.position;
                    const float distance2 = glm::length2(to_interactable);
                    if (distance2 > closest_distance2)
                        continue;

                    // Skip the facing check right on top of the interactable, where the direction
                    // to it is undefined. Facing is already normalized, so only to_interactable needs it.
                    if (distance2 > 1e-10f && glm::dot(glm::normalize(to_interactable), facing.direction) < kInteractionFacingCosThreshold)
                        continue;

                    closest_entity = interactable_entity;
                    closest_distance2 = distance2;
                }

                if (closest_entity != entt::null)
                {
                    raise_event<components::InteractEvent>(closest_entity, player_entity);
                }
            }
        }

        // Re-read after acting: if should_search just opened a message, that must count as
        // "open" for next frame, even though message_was_open was false when read above.
        _message_was_open_previous_frame = !_registry.ctx().get<components::MessageBoxState>().lines.empty();
    }
} // namespace tilegame::systems
