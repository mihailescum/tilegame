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

        engine::ResourceManager &resourceManager;

        void loadCharacters() const;

        void createMapEntity(engine::Map &map) const;
        engine::Entity createCharacterEntity(engine::Character &character) const;

    public:
        ContentSystem(WorldScene &scene, engine::ResourceManager &resourceManager);
        void initialize();
        void loadContent() const;
        void unloadContent() const;
    };
} // namespace tilegame::worldscene