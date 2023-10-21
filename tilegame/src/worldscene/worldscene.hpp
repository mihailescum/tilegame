#pragma once

#include <string>
#include <unordered_map>

#include "engine.hpp"

namespace tilegame
{
    class Tilegame;
}

namespace tilegame::worldscene
{
    class WorldScene : public engine::Scene
    {
    private:
        /*engine::SpriteBatch &spriteBatch;
        std::unordered_map<std::string, engine::Entity> taggedEntities;

        std::unique_ptr<ContentSystem> contentSystem;
        std::unique_ptr<InputSystem> inputSystem;
        std::unique_ptr<CameraSystem> cameraSystem;
        std::unique_ptr<MovementSystem> movementSystem;
        std::unique_ptr<RenderSystem> renderSystem;
        std::unique_ptr<AnimationSystem> animationSystem;
        std::unique_ptr<SpriteSystem> spriteSystem;

        engine::Entity *findByObjectId(const std::string &object_id);*/
        engine::Texture2D *_test_tex;

        void createSystems();
        engine::SpriteBatch &get_spritebatch();

    public:
        WorldScene(Tilegame &game);
        ~WorldScene() = default;

        virtual void initialize() override;
        virtual void load_content() override;
        virtual void unload_content() override;
        virtual void update(double gametime) override;
        virtual void draw() override;
    };
} // namespace tilegame::worldscene