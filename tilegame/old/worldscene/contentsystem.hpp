#pragma once

#include "engine.hpp"

namespace tilegame::worldscene
{
    class WorldScene;

    class ContentSystem
    {
    private:
        WorldScene &scene;
        entt::registry &registry;

        engine::ResourceManager &_resource_manager;

        void loadCharacters() const;

        void createMapEntity(engine::Map &map) const;
        engine::Entity createCharacterEntity(engine::Character &character) const;

    public:
        ContentSystem(WorldScene &scene, engine::ResourceManager &_resource_manager);
        void initialize();
        void load_content() const;
        void unload_content() const;
    };
} // namespace tilegame::worldscene