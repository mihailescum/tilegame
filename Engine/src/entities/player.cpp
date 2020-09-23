#include "entities/player.hpp"

namespace engine
{
    Player::Player(Entity *entity, const Viewport &viewport) : entity(entity), camera(new Camera(viewport))
    {
    }

    const Camera &Player::getCamera() const
    {
        return *this->camera;
    }

    void Player::update(const double deltaTime)
    {
        this->entity->update(deltaTime);
        this->camera->setPosition(this->entity->getPosition());
    }

    void Player::moveLeft()
    {
        this->entity->moveLeft();
    }

    void Player::moveRight()
    {
        this->entity->moveRight();
    }

    void Player::moveUp()
    {
        this->entity->moveUp();
    }

    void Player::moveDown()
    {
        this->entity->moveDown();
    }

} // namespace engine