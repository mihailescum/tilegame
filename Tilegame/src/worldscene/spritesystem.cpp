#include "worldscene/spritesystem.hpp"

#include "worldscene/worldscene.hpp"

namespace tilegame::worldscene
{
    SpriteSystem::SpriteSystem(WorldScene &worldscene) : worldscene(worldscene), registry(worldscene.getRegistry()) {}

    void SpriteSystem::initialize()
    {
        this->spriteInfoObserver = std::make_unique<engine::Observer>(this->registry);
        this->spriteInfoObserver->connectOnUpdate<engine::SpriteInfoComponent>();
    }

    void SpriteSystem::update()
    {
        auto spriteInfos = registry.view<engine::SpriteInfoComponent>();
        for (const auto &entity : *this->spriteInfoObserver)
        {
            const auto &spriteInfo = spriteInfos.get(entity);
            if (registry.has<engine::SpriteComponent>(entity))
            {
                registry.patch<engine::SpriteComponent>(
                    entity,
                    [=](auto &sprite) {
                        sprite.sourceRectangle = spriteInfo.spriteInfo->spriteStates.at(spriteInfo.currentState)[spriteInfo.currentFrame];
                    });
            }
        }
        this->spriteInfoObserver->clear();
    }
} // namespace tilegame::worldscene