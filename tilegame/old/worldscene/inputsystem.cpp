#include "worldscene/inputsystem.hpp"

#include "worldscene/worldscene.hpp"

namespace tilegame::worldscene
{
    InputSystem::InputSystem(WorldScene &scene, const engine::Window &window) : registry(scene.getRegistry()), window(window) {}

    void InputSystem::initialize() {}

    void InputSystem::processInput()
    {
        auto inputs = this->registry.view<engine::InputComponent>();
        for (auto entity : inputs)
        {
            engine::InputComponent &input = inputs.get(entity);
            for (auto kvp : input.keys)
            {
                kvp.second.second = kvp.second.first;
                kvp.second.first = this->window.is_key_pressed(kvp.first);

                // If the entity has a camera attached to it, it is the player
                if (this->registry.has<engine::CameraComponent, engine::MoveComponent>(entity))
                    handlePlayerInput(entity, kvp.first, kvp.second);
            }
        }
    }

    void InputSystem::handlePlayerInput(const entt::entity &entity, const int key, const std::pair<bool, bool> keyPressed)
    {
        engine::MoveComponent &moveComponent = this->registry.get<engine::MoveComponent>(entity);
        if (key == GLFW_KEY_DOWN && keyPressed.first)
        {
            moveComponent.direction |= engine::MoveComponent::MoveDirection::Down;
        }
        else if (key == GLFW_KEY_UP && keyPressed.first)
        {
            moveComponent.direction |= engine::MoveComponent::MoveDirection::Up;
        }
        else if (key == GLFW_KEY_LEFT && keyPressed.first)
        {
            moveComponent.direction |= engine::MoveComponent::MoveDirection::Left;
        }
        else if (key == GLFW_KEY_RIGHT && keyPressed.first)
        {
            moveComponent.direction |= engine::MoveComponent::MoveDirection::Right;
        }
    }
} // namespace tilegame::worldscene