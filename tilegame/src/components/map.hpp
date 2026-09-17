#pragma once

#include "sol/sol.hpp"

namespace tilegame::components
{
    /**
     * @brief Empty tag marking an entity as a map, together with a Transform (its position in
     * world space) and a Shape (its pixel-space bounds, relative to that Transform) - see
     * systems::World::to_global()/map_at(). Created once per map by
     * content/scripts/maploader.lua, the ECS-native replacement for the former
     * engine::tilemap::World resource. Exposed to Lua as `_Map`.
     */
    struct Map
    {
        static void register_component(sol::state &lua);
    };
} // namespace tilegame::components
