#pragma once

#include "engine.hpp"

namespace tilegame::worldscene
{
    class ContentSystem
    {
    private:
        engine::Scene &scene;
        engine::ResourceManager &resourceManager;

        void createMapEntities(engine::Map &map) const;

    public:
        ContentSystem(engine::Scene &scene, engine::ResourceManager &resourceManager) : scene(scene), resourceManager(resourceManager) {}
        void initialize();
        void loadContent() const;
        void unloadContent() const;
    };
} // namespace tilegame::worldscene