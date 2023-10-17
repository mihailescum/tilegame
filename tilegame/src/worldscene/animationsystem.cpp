#include "worldscene/animationsystem.hpp"

#include <string>

#include "worldscene/worldscene.hpp"

namespace tilegame::worldscene
{
    AnimationSystem::AnimationSystem(WorldScene &worldscene) : worldscene(worldscene), registry(worldscene.getRegistry()) {}

    void AnimationSystem::initialize() {}

    void AnimationSystem::update(const double deltaTime)
    {
        this->updateComponents(deltaTime);
        this->updateVisibleSprites();
    }

    void AnimationSystem::updateComponents(const double deltaTime)
    {
        auto animations = this->registry.view<engine::AnimationComponent>();

        for (const auto &entity : animations)
        {
            auto &animation = animations.get(entity);
            animation.timePassed += deltaTime;
            if (animation.timePassed >= animation.tickDuration)
            {
                animation.doAnimation = true;
                animation.timePassed -= animation.tickDuration;
            }
        }
    }

    void AnimationSystem::updateVisibleSprites()
    {
        auto sprites = registry.view<engine::AnimationComponent, engine::SpriteInfoComponent, engine::VisibilityComponent>();
        for (const auto &entity : sprites)
        {
            auto [animation, spriteInfo] = sprites.get<engine::AnimationComponent, engine::SpriteInfoComponent>(entity);
            if (animation.doAnimation)
            {
                registry.patch<engine::SpriteInfoComponent>(
                    entity,
                    [=](auto &spriteInfo) {
                        spriteInfo.currentFrame++;
                        const std::string &currentState = spriteInfo.currentState;
                        if (spriteInfo.currentFrame >= spriteInfo.spriteInfo->spriteStates.at(currentState).size())
                            spriteInfo.currentFrame = 0;
                    });
                animation.doAnimation = false;
            }
        }
    }
} // namespace tilegame::worldscene