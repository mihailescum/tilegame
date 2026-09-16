#pragma once

#include <string>

#include "sol/sol.hpp"

namespace tilegame::components
{
    // State attached to the single lightning entity (see systems::Lightning), alongside its
    // Timer (which schedules the next strike) and EventListener<TimerEvent> (which reacts to
    // it). Remembered separately from the Timer itself so each strike can reschedule to a fresh
    // random interval within [min_interval, max_interval) seconds, and know how long its screen
    // flash should take to decay (flash_duration), without systems::Lightning needing to know
    // anything about Timer beyond emplace_or_replace.
    struct Lightning
    {
        float min_interval;
        float max_interval;
        float flash_duration;
    };

    // ---- Command: emplaced on a fresh entity by the _set_lightning Lua binding (see
    // Script::set_lightning()), consumed and destroyed by systems::Lightning the next time it
    // updates. (Re)starts recurring lightning strikes, each waiting a fresh random interval
    // within [min_interval, max_interval) seconds after the previous one and, once struck,
    // brightening the scene for flash_duration seconds.
    struct SetLightningEvent
    {
        float min_interval;
        float max_interval;
        float flash_duration;
    };

    // ---- Command: emplaced on a fresh entity by the _clear_lightning Lua binding (see
    // Script::clear_lightning()), consumed and destroyed by systems::Lightning the next time it
    // updates. Stops the recurring schedule; an in-progress screen flash still finishes playing
    // out, since that's driven by its own independent decay, not the Timer.
    struct ClearLightningEvent
    {
    };

    /**
     * @brief Entt event component raised for one frame by systems::Lightning each time a
     * strike's random interval elapses, and delivered to `EventListener<LightningEvent>`
     * components via the engine's event-listener mechanism before being cleared. Exposed to Lua
     * as `_LightningEvent`.
     */
    struct LightningEvent
    {
        inline static const std::string EVENT_TYPE = "LIGHTNING_EVENT";

        LightningEvent() = default;
        [[nodiscard]] std::string to_string() const;

        static void register_component(sol::state &lua);
    };
} // namespace tilegame::components
