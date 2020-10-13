#include "worldscene/contentsystem.hpp"

#include "engine.hpp"

namespace tilegame::worldscene
{
    void ContentSystem::initialize() {}

    void ContentSystem::loadContent() const
    {
        engine::Map *map1 = resourceManager.loadResource<engine::Map>("map1", "content/world/map1.tmx", this);
    }

    void ContentSystem::unloadContent() const
    {
    }
} // namespace tilegame