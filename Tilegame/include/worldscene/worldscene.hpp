#pragma once

#include <string>
#include <unordered_map>
#include "entt.hpp"

#include "engine.hpp"

#include "tilegame.hpp"
#include "worldscene/contentsystem.hpp"
#include "worldscene/inputsystem.hpp"
#include "worldscene/camerasystem.hpp"
#include "worldscene/movementsystem.hpp"
#include "worldscene/rendersystem.hpp"
#include "worldscene/animationsystem.hpp"
#include "worldscene/spritesystem.hpp"

namespace tilegame::worldscene
{
    class WorldScene : public engine::Scene
    {
    private:
        engine::SpriteBatch &spriteBatch;
        std::unordered_map<std::string, engine::Entity> taggedEntities;

        std::unique_ptr<ContentSystem> contentSystem;
        std::unique_ptr<InputSystem> inputSystem;
        std::unique_ptr<CameraSystem> cameraSystem;
        std::unique_ptr<MovementSystem> movementSystem;
        std::unique_ptr<RenderSystem> renderSystem;
        std::unique_ptr<AnimationSystem> animationSystem;
        std::unique_ptr<SpriteSystem> spriteSystem;

        void createSystems();
        engine::Entity *findByObjectId(const std::string &object_id);

    public:
        WorldScene(Tilegame &game);

        virtual void initialize() override;
        virtual void loadContent() override;
        virtual void unloadContent() override;
        virtual void processInput() override;
        virtual void update(const double deltaTime) override;
        virtual void draw() override;

        void E_show(const std::string &objectId, const bool show);
        void E_setPosition(const std::string &objectId, const float x, const float y);
    };
} // namespace tilegame