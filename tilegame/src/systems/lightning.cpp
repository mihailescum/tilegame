#include "lightning.hpp"

#include <algorithm>

#include "helper.hpp"

#include "components/lightning.hpp"
#include "components/timer.hpp"
#include "components/event.hpp"
#include "components/inactive.hpp"

namespace tilegame::systems
{
    namespace
    {
        using namespace entt::literals;
        // Registry context id (registry.ctx()) under which the single lightning entity's
        // handle is stored, for the entire program's life once load_content() has run. Private
        // to this file; nothing outside systems::Lightning needs to find it.
        constexpr auto LIGHTNING_ENTITY_ID = "lightning_entity"_hs;
    }

    Lightning::Lightning(tilegame::Scene &scene, entt::registry &registry)
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
        _registry.emplace<components::Inactive>(entity);

        // Source-filtered to itself, so it only reacts to the TimerEvent its own Timer raises
        // and not some unrelated Timer elsewhere in the game.
        _registry.emplace<components::EventListener<components::TimerEvent>>(
            entity,
            [this, entity](const std::string &, const components::TimerEvent &, entt::entity)
            { strike(entity); },
            entity);

        _registry.ctx().emplace_as<entt::entity>(LIGHTNING_ENTITY_ID, entity);
    }

    void Lightning::strike(entt::entity lightning_entity)
    {
        const auto &lightning = _registry.get<components::Lightning>(lightning_entity);

        _flash_intensity = 1.0f;
        _flash_duration = lightning.flash_duration;

        const auto event_entity = _registry.create();
        _registry.emplace<components::LightningEvent>(event_entity);
        raise_events<components::LightningEvent>();
        _registry.destroy(event_entity);

        _registry.emplace_or_replace<components::Timer>(lightning_entity, get_random(lightning.min_interval, lightning.max_interval), false);
    }

    void Lightning::apply_pending_commands()
    {
        const auto lightning_entity = _registry.ctx().get<entt::entity>(LIGHTNING_ENTITY_ID);

        for (auto &&[entity, event] : _registry.view<components::SetLightningEvent>().each())
        {
            _registry.replace<components::Lightning>(lightning_entity, event.min_interval, event.max_interval, event.flash_duration);
            _registry.emplace_or_replace<components::Timer>(lightning_entity, get_random(event.min_interval, event.max_interval), false);
            _registry.remove<components::Inactive>(lightning_entity);
            _registry.destroy(entity);
        }

        for (const auto entity : _registry.view<components::ClearLightningEvent>())
        {
            _registry.emplace_or_replace<components::Inactive>(lightning_entity);
            _registry.destroy(entity);
        }
    }

    void Lightning::update(const engine::GameTime &update_time)
    {
        apply_pending_commands();

        _flash_intensity = _flash_duration > 0.0f
                                ? std::max(0.0f, _flash_intensity - update_time.elapsed_time / _flash_duration)
                                : 0.0f;

        _daytime_shader->use();
        _daytime_shader->set("flash_intensity", _flash_intensity);
    }
} // namespace tilegame::systems
