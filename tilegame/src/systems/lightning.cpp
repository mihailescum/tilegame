#include "lightning.hpp"

#include <algorithm>

#include "helper.hpp"

#include "components/lightning.hpp"
#include "components/timer.hpp"

namespace tilegame::systems
{
    Lightning::Lightning(engine::Scene &scene, entt::registry &registry)
        : System(scene, registry), _flash_intensity(0.0f), _flash_duration(0.0f)
    {
    }

    void Lightning::load_content()
    {
        // Shared with systems::Daytime, which is the one that loads it (see WorldScene::load_content(),
        // where Daytime::load_content() runs first).
        _daytime_shader = &_scene.game().resource_manager().get<engine::Shader>("daytime_shader");
        _daytime_shader->use();
        _daytime_shader->set("flash_intensity", 0.0f);

        const auto entity = _registry.create();
        _registry.emplace<components::Lightning>(entity, 0.0f, 0.0f, 0.0f);
        _registry.emplace<engine::Inactive>(entity);

        // Source-filtered to itself, so it only reacts to the TimerEvent its own Timer raises
        // and not some unrelated Timer elsewhere in the game.
        _registry.emplace<engine::EventListener<components::TimerEvent>>(
            entity,
            [this, entity](const std::string &, const components::TimerEvent &, entt::entity, entt::entity)
            { strike(entity); },
            entity);

        // On a *separate* entity - never tagged Inactive - since `entity` starts Inactive and
        // raise_event()'s listener view excludes Inactive entities: attaching these to `entity`
        // itself would mean the very event whose job is to remove Inactive could never reach it.
        const auto control_entity = _registry.create();
        _registry.emplace<engine::EventListener<components::SetLightningEvent>>(
            control_entity,
            [this, entity](const std::string &, const components::SetLightningEvent &event, entt::entity, entt::entity)
            {
                _registry.replace<components::Lightning>(entity, event.min_interval, event.max_interval, event.flash_duration);
                _registry.emplace_or_replace<components::Timer>(entity, get_random(event.min_interval, event.max_interval), false);
                _registry.remove<engine::Inactive>(entity);
            },
            entt::null);
        _registry.emplace<engine::EventListener<components::ClearLightningEvent>>(
            control_entity,
            [this, entity](const std::string &, const components::ClearLightningEvent &, entt::entity, entt::entity)
            { _registry.emplace_or_replace<engine::Inactive>(entity); },
            entt::null);
    }

    void Lightning::strike(entt::entity lightning_entity)
    {
        const auto &lightning = _registry.get<components::Lightning>(lightning_entity);

        _flash_intensity = 1.0f;
        _flash_duration = lightning.flash_duration;

        raise_event<components::LightningEvent>();

        _registry.emplace_or_replace<components::Timer>(lightning_entity, get_random(lightning.min_interval, lightning.max_interval), false);
    }

    void Lightning::update(const engine::GameTime &update_time)
    {
        _flash_intensity = _flash_duration > 0.0f
                               ? std::max(0.0f, _flash_intensity - update_time.elapsed_time / _flash_duration)
                               : 0.0f;

        _daytime_shader->use();
        _daytime_shader->set("flash_intensity", _flash_intensity);
    }
} // namespace tilegame::systems
