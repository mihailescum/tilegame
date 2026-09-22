#include "camera.hpp"

#include <cmath>
#include <algorithm>

#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>

#include "helper.hpp"

#include "components/camera.hpp"
#include "components/transform.hpp"
#include "components/depthorigin.hpp"
#include "components/camerashake.hpp"
#include "components/player.hpp"
#include "components/pin.hpp"
#include "components/timer.hpp"
#include "components/event.hpp"
#include "components/inactive.hpp"

namespace tilegame::systems
{
    namespace
    {
        using namespace entt::literals;
        // Registry context ids (registry.ctx()) under which the two shake axis entities' handles
        // are stored, for the entire program's life once load_content() has run. Private to this
        // file - unlike components::CAMERA_ENTITY_ID, nothing outside systems::Camera needs to
        // find these.
        constexpr auto HORIZONTAL_SHAKE_ENTITY_ID = "camera_shake_horizontal_entity"_hs;
        constexpr auto VERTICAL_SHAKE_ENTITY_ID = "camera_shake_vertical_entity"_hs;

        // How often a shake axis picks a new random jitter target while its Timer is still
        // running. Purely a timing knob - the jitter's strength is components::CameraShakeAxis::offset,
        // given by the caller, not derived from this.
        constexpr float SHAKE_RETARGET_INTERVAL = 0.05f;
        // Once a shake axis is settling (its Timer has rung), how close to 0 `current_offset`
        // must get before the axis is considered fully at rest again and tagged Inactive.
        constexpr float SHAKE_SETTLE_EPSILON = 0.1f;
    }

    Camera::Camera(tilegame::Scene &scene, entt::registry &registry) : System(scene, registry)
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
        auto players = _registry.view<const components::Player>(entt::exclude<components::Inactive>);
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

        const auto horizontal_axis_entity = create_shake_axis_entity();
        _registry.ctx().emplace_as<entt::entity>(HORIZONTAL_SHAKE_ENTITY_ID, horizontal_axis_entity);

        const auto vertical_axis_entity = create_shake_axis_entity();
        _registry.ctx().emplace_as<entt::entity>(VERTICAL_SHAKE_ENTITY_ID, vertical_axis_entity);

        // On a *separate* entity - never tagged Inactive - since both axis entities start
        // Inactive and raise_event()'s listener view excludes Inactive entities: attaching
        // these to the axis entities themselves would mean the very event whose job is to
        // remove Inactive could never reach them.
        const auto control_entity = _registry.create();
        _registry.emplace<components::EventListener<components::ShakeCameraHorizontalEvent>>(
            control_entity,
            [this, horizontal_axis_entity](const std::string &, const components::ShakeCameraHorizontalEvent &event, entt::entity, entt::entity)
            {
                _registry.replace<components::CameraShakeAxis>(horizontal_axis_entity, event.displacement_speed, event.offset, 0.0f, 0.0f, 0.0f, false);
                _registry.emplace_or_replace<components::Timer>(horizontal_axis_entity, event.duration, false);
                _registry.remove<components::Inactive>(horizontal_axis_entity);
            },
            entt::null);
        _registry.emplace<components::EventListener<components::ShakeCameraVerticalEvent>>(
            control_entity,
            [this, vertical_axis_entity](const std::string &, const components::ShakeCameraVerticalEvent &event, entt::entity, entt::entity)
            {
                _registry.replace<components::CameraShakeAxis>(vertical_axis_entity, event.displacement_speed, event.offset, 0.0f, 0.0f, 0.0f, false);
                _registry.emplace_or_replace<components::Timer>(vertical_axis_entity, event.duration, false);
                _registry.remove<components::Inactive>(vertical_axis_entity);
            },
            entt::null);
    }

    entt::entity Camera::create_shake_axis_entity()
    {
        const auto entity = _registry.create();
        _registry.emplace<components::CameraShakeAxis>(entity, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, false);
        _registry.emplace<components::Inactive>(entity);

        // Source-filtered to itself, so it only reacts to the TimerEvent its own Timer raises
        // and not some unrelated Timer elsewhere in the game - the same native-event mechanism
        // Lua subscribes to via _add_event_listener.
        _registry.emplace<components::EventListener<components::TimerEvent>>(
            entity,
            [this, entity](const std::string &, const components::TimerEvent &, entt::entity, entt::entity)
            { _registry.get<components::CameraShakeAxis>(entity).settling = true; },
            entity);

        return entity;
    }

    float Camera::update_shake_axis(entt::entity axis_entity, float elapsed_time)
    {
        if (_registry.all_of<components::Inactive>(axis_entity))
        {
            return 0.0f;
        }

        auto &shake = _registry.get<components::CameraShakeAxis>(axis_entity);

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
            // Its EventListener<TimerEvent> already fired this frame (see
            // create_shake_axis_entity()): head smoothly back to center instead of jittering
            // further.
            shake.target_offset = 0.0f;
        }

        const float delta = shake.target_offset - shake.current_offset;
        const float max_step = shake.displacement_speed * elapsed_time;
        shake.current_offset += std::clamp(delta, -max_step, max_step);

        if (shake.settling && std::abs(shake.current_offset) < SHAKE_SETTLE_EPSILON)
        {
            shake.current_offset = 0.0f;
            _registry.emplace<components::Inactive>(axis_entity);
        }

        return shake.current_offset;
    }

    void Camera::update(const engine::GameTime &update_time)
    {
        const auto camera_entity = _registry.ctx().get<entt::entity>(components::CAMERA_ENTITY_ID);
        auto &camera = _registry.get<components::Camera>(camera_entity);
        const auto &transform = _registry.get<const components::Transform>(camera_entity);

        const glm::vec2 shake_offset(
            update_shake_axis(_registry.ctx().get<entt::entity>(HORIZONTAL_SHAKE_ENTITY_ID), update_time.elapsed_time),
            update_shake_axis(_registry.ctx().get<entt::entity>(VERTICAL_SHAKE_ENTITY_ID), update_time.elapsed_time));

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
