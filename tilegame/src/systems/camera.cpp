#include "camera.hpp"

#include <cmath>
#include <algorithm>
#include <vector>

#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>

#include "helper.hpp"

#include "components/camera.hpp"
#include "components/transform.hpp"
#include "components/depthorigin.hpp"
#include "components/camerashake.hpp"
#include "components/player.hpp"
#include "components/pin.hpp"

namespace tilegame::systems
{
    namespace
    {
        // How often a shake axis picks a new random jitter target while it's still running.
        // Purely a timing knob - the jitter's strength is components::CameraShakeAxis::offset,
        // given by the caller, not derived from this.
        constexpr float SHAKE_RETARGET_INTERVAL = 0.05f;
        // Once a shake axis is settling (its duration has run out), how close to 0
        // `current_offset` must get before the axis is considered fully at rest again.
        constexpr float SHAKE_SETTLE_EPSILON = 0.1f;
    }

    Camera::Camera(engine::Scene &scene, entt::registry &registry) : System(scene, registry)
    {
    }

    void Camera::initialize()
    {
    }

    void Camera::load_content()
    {
        const auto camera_entity = _registry.create();
        _registry.emplace<components::Camera>(
            camera_entity,
            1.0,
            glm::mat4(1.0),
            _scene.game().graphicsdevice().viewport());
        _registry.emplace<components::Transform>(camera_entity, glm::vec2(0.0, 0.0));
        _registry.emplace<components::DepthOrigin>(camera_entity, 0.0f);

        entt::entity player1_entity = entt::null;
        auto players = _registry.view<const components::Player>(entt::exclude<engine::Inactive>);
        for (auto &&[entity, player] : players.each())
        {
            if (player.id == 1)
            {
                player1_entity = entity;
                break;
            }
        }

        if (player1_entity != entt::null)
        {
            _registry.emplace<components::Pin>(camera_entity, player1_entity);
        }

        _registry.ctx().emplace_as<entt::entity>(components::CAMERA_ENTITY_ID, camera_entity);

        // On a *separate*, never Inactive entity, following the same convention as
        // systems::Lightning/Weather's control entities.
        const auto control_entity = _registry.create();
        _registry.emplace<engine::EventListener<components::ShakeCameraEvent>>(
            control_entity,
            [this](const std::string &, const components::ShakeCameraEvent &event, entt::entity, entt::entity shake_entity)
            {
                if (!_registry.valid(shake_entity))
                {
                    return;
                }

                auto &shake = _registry.emplace_or_replace<components::CameraShake>(shake_entity);
                if (event.horizontal)
                {
                    shake.horizontal.emplace(*event.horizontal);
                }
                if (event.vertical)
                {
                    shake.vertical.emplace(*event.vertical);
                }
            },
            entt::null);
        _registry.emplace<engine::EventListener<components::StopCameraShakeEvent>>(
            control_entity,
            [this](const std::string &, const components::StopCameraShakeEvent &, entt::entity, entt::entity shake_entity)
            {
                if (!_registry.valid(shake_entity) || !_registry.all_of<components::CameraShake>(shake_entity))
                {
                    return;
                }

                auto &shake = _registry.get<components::CameraShake>(shake_entity);
                for (auto *axis : {&shake.horizontal, &shake.vertical})
                {
                    if (*axis)
                    {
                        (*axis)->settling = true;
                    }
                }
            },
            entt::null);
    }

    float Camera::update_shake_axis(std::optional<components::CameraShakeAxis> &axis, float elapsed_time)
    {
        if (!axis)
        {
            return 0.0f;
        }

        auto &shake = *axis;

        if (!shake.settling)
        {
            shake.remaining_duration -= elapsed_time;
            if (shake.remaining_duration <= 0.0f)
            {
                shake.settling = true;
            }
        }

        if (!shake.settling)
        {
            shake.retarget_clock -= elapsed_time;
            if (shake.retarget_clock <= 0.0f)
            {
                shake.target_offset = get_random(-shake.offset, shake.offset);
                shake.retarget_clock += SHAKE_RETARGET_INTERVAL;
            }
        }
        else
        {
            // Duration ran out or the shake was stopped: head smoothly back to center instead
            // of jittering further.
            shake.target_offset = 0.0f;
        }

        const float delta = shake.target_offset - shake.current_offset;
        const float max_step = shake.displacement_speed * elapsed_time;
        shake.current_offset += std::clamp(delta, -max_step, max_step);

        if (shake.settling && std::abs(shake.current_offset) < SHAKE_SETTLE_EPSILON)
        {
            axis.reset();
            return 0.0f;
        }

        return shake.current_offset;
    }

    glm::vec2 Camera::update_shakes(float elapsed_time)
    {
        glm::vec2 offset(0.0f);
        std::vector<entt::entity> finished_shakes;

        for (auto &&[entity, shake] : _registry.view<components::CameraShake>().each())
        {
            offset.x += update_shake_axis(shake.horizontal, elapsed_time);
            offset.y += update_shake_axis(shake.vertical, elapsed_time);

            if (!shake.horizontal && !shake.vertical)
            {
                finished_shakes.push_back(entity);
            }
        }

        // Only after the loop: a Lua listener may start a new shake, which would emplace a
        // CameraShake while the view above is still iterating.
        for (const auto entity : finished_shakes)
        {
            raise_event<components::CameraShakeEndedEvent>(entity);
            if (_registry.valid(entity))
            {
                _registry.destroy(entity);
            }
        }

        return offset;
    }

    void Camera::update(const engine::GameTime &update_time)
    {
        const auto camera_entity = _registry.ctx().get<entt::entity>(components::CAMERA_ENTITY_ID);
        auto &camera = _registry.get<components::Camera>(camera_entity);
        const auto &transform = _registry.get<const components::Transform>(camera_entity);

        const glm::vec2 shake_offset = update_shakes(update_time.elapsed_time);

        glm::vec2 position = transform.position + shake_offset;
        float scale = camera.scale;

        glm::vec3 translate(
            floor(-(position.x - camera.viewport.dimensions.x / 2) * scale) / scale,
            floor(-(position.y - camera.viewport.dimensions.y / 2) * scale) / scale,
            0.0);

        // TODO use patch
        camera.transform = glm::translate(glm::mat4(1.0), translate);
        camera.transform = glm::scale(camera.transform, glm::vec3(scale));

        // World-space rect visible through this camera, derived by mapping the viewport's
        // screen-space corners back through the inverse of the transform just computed above -
        // kept in sync with it rather than re-derived from position/scale/viewport directly, so
        // it can't drift if the transform math above ever changes.
        const glm::mat4 inverse_transform = glm::inverse(camera.transform);
        const glm::vec2 top_left = glm::vec2(inverse_transform * glm::vec4(0.0f, 0.0f, 0.0f, 1.0f));
        const glm::vec2 bottom_right = glm::vec2(inverse_transform * glm::vec4(
                                                                         static_cast<float>(camera.viewport.dimensions.x),
                                                                         static_cast<float>(camera.viewport.dimensions.y),
                                                                         0.0f, 1.0f));
        camera.visible_bounds = engine::Rectangle(top_left, bottom_right - top_left);

        // See components::DepthOrigin - snapped to a coarse grid (not just set to `position.y`
        // directly) so it only actually changes when the camera crosses a chunk boundary, rather
        // than drifting by a tiny amount every frame the camera moves at all.
        auto &depth_origin = _registry.get<components::DepthOrigin>(camera_entity);
        depth_origin.y = std::floor(position.y / components::DepthOrigin::CHUNK_SIZE) * components::DepthOrigin::CHUNK_SIZE;
    }
} // namespace tilegame::systems
