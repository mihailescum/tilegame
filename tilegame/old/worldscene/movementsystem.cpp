#include "worldscene/movementsystem.hpp"

#include "entt.hpp"

#include "engine.hpp"

#include "worldscene/worldscene.hpp"

namespace tilegame::worldscene
{
    struct MovementSystem::Impl
    {
        Impl(entt::registry &registry);
        ~Impl() = default;

        entt::registry &registry;

        // Observes all entities where the CollisionComponent was updated, i.e. entities for which collision detction has to be performed
        std::unique_ptr<engine::Observer> collisionObserver;

        // Stores collision times of entities. Clear after each collision handling
        std::vector<double> collisionsX;
        std::vector<double> collisionsY;

        glm::dvec2 computeDirectionVector(const engine::MoveComponent::MoveDirection &direction) const;
        void handleCollisions(const double deltaTime);
        bool collidingRayAABB(const glm::dvec2 &rayStart, const glm::dvec2 &rayDirection, const engine::Rectangle &aabb, glm::dvec2 &contactPoint, glm::dvec2 &contactNormal, double &contactTime) const;
        bool collidingMovingAABBMovingAABB(const engine::Rectangle &a, const glm::dvec2 &movementA, const engine::Rectangle &b, const glm::dvec2 &movementB, const double deltaTime, glm::dvec2 &contactPoint, glm::dvec2 &contactNormal, double &contactTime) const;

        void initialize();
        void update(const double deltaTime);
        void set_position(const entt::entity &entity, const double x, const double y);
    };

    MovementSystem::MovementSystem(WorldScene &scene) : impl(std::make_unique<MovementSystem::Impl>(scene.getRegistry())) {}
    MovementSystem::~MovementSystem() = default;
    void MovementSystem::initialize() { impl->initialize(); }
    void MovementSystem::update(const double deltaTime) { impl->update(deltaTime); }
    void MovementSystem::set_position(const entt::entity &entity, const double x, const double y) { impl->set_position(entity, x, y); }

    MovementSystem::Impl::Impl(entt::registry &registry) : registry(registry) {}

    void MovementSystem::Impl::initialize()
    {
        collisionObserver = std::make_unique<engine::Observer>(registry);
        collisionObserver->connectOnUpdate<engine::CollisionComponent>();
    }

    void MovementSystem::Impl::update(const double deltaTime)
    {
        auto moveables = registry.view<engine::MoveComponent, engine::PositionComponent>();
        for (auto entity : moveables)
        {
            engine::MoveComponent &moveComponent = moveables.get<engine::MoveComponent>(entity);
            const engine::PositionComponent &positionComponent = moveables.get<engine::PositionComponent>(entity);

            glm::dvec2 direction = computeDirectionVector(moveComponent.direction);
            moveComponent.direction = engine::MoveComponent::MoveDirection::None;

            if (static_cast<int>(entity) == 2)
            {
                moveComponent.direction = engine::MoveComponent::MoveDirection::Right | engine::MoveComponent::MoveDirection::Down;
                moveComponent.velocity = 100.0;
            }

            if (glm::any(glm::greaterThan(glm::abs(direction), glm::dvec2(1e-10))))
            {
                direction = glm::normalize(direction);
                registry.patch<engine::CollisionComponent>(
                    entity,
                    [=](auto &collisionComponent) {
                        collisionComponent.movementVector = direction * moveComponent.velocity;
                    });
            }
        }

        handleCollisions(deltaTime);
    }

    glm::dvec2 MovementSystem::Impl::computeDirectionVector(const engine::MoveComponent::MoveDirection &direction) const
    {
        glm::dvec2 result(0.0);
        if (static_cast<int>(direction & engine::MoveComponent::MoveDirection::Left))
            result.x -= 1.0;
        if (static_cast<int>(direction & engine::MoveComponent::MoveDirection::Right))
            result.x += 1.0;
        if (static_cast<int>(direction & engine::MoveComponent::MoveDirection::Up))
            result.y -= 1.0;
        if (static_cast<int>(direction & engine::MoveComponent::MoveDirection::Down))
            result.y += 1.0;
        return result;
    }

    void MovementSystem::Impl::handleCollisions(const double deltaTime)
    {
        const auto &colliders = registry.view<engine::CollisionComponent, engine::PositionComponent>();
        for (const auto &entity : *collisionObserver)
        {
            if (!registry.has<engine::PositionComponent>(entity))
                continue;

            auto [collisionComponent, positionComponent] = colliders.get<engine::CollisionComponent, engine::PositionComponent>(entity);

            collisionsX.clear();
            collisionsY.clear();

            std::vector<std::pair<double, glm::vec2>> collisions;
            for (const auto &second : colliders)
            {
                if (entity == second)
                    continue;

                auto [collisionComponentSecond, positionComponentSecond] = colliders.get<engine::CollisionComponent, engine::PositionComponent>(second);

                if (registry.has<engine::BoundingBoxComponent>(entity) && registry.has<engine::BoundingBoxComponent>(second))
                {
                    engine::Log::d(static_cast<int>(entity), "  ", static_cast<int>(second));

                    const auto &boundingBoxComponent = registry.get<engine::BoundingBoxComponent>(entity);
                    const auto &boundingBoxComponentSecond = registry.get<engine::BoundingBoxComponent>(second);

                    engine::Rectangle boundingBox = boundingBoxComponent;
                    boundingBox.x += positionComponent.x();
                    boundingBox.y += positionComponent.y();
                    engine::Rectangle boundingBoxSecond = boundingBoxComponentSecond;
                    boundingBoxSecond.x += positionComponentSecond.x();
                    boundingBoxSecond.y += positionComponentSecond.y();

                    glm::dvec2 contactPoint, contactNormal;
                    double contactTime;
                    if (collidingMovingAABBMovingAABB(
                            boundingBox, collisionComponent.movementVector,
                            boundingBoxSecond, collisionComponentSecond.movementVector,
                            deltaTime, contactPoint, contactNormal, contactTime))
                    {
                        if ((sgn(contactNormal.x) > 0 && sgn(collisionComponent.movementVector.x) < 0) ||
                            (sgn(contactNormal.x) < 0 && sgn(collisionComponent.movementVector.x) > 0))
                            collisionsX.push_back(contactTime);

                        if ((sgn(contactNormal.y) > 0 && sgn(collisionComponent.movementVector.y) < 0) ||
                            (sgn(contactNormal.y) < 0 && sgn(collisionComponent.movementVector.y) > 0))
                            collisionsY.push_back(contactTime);

                        collisions.push_back({contactTime, contactNormal});
                    }
                }
            }

            std::sort(collisionsX.begin(), collisionsX.end(), [](const double &a, const double &b) { return a < b; });
            std::sort(collisionsY.begin(), collisionsY.end(), [](const double &a, const double &b) { return a < b; });

            std::sort(collisions.begin(), collisions.end(),
                      [](const auto &a, const auto &b) { return a.first < b.first; });

            for (auto &pair : collisions)
            {
                glm::dvec2 old = collisionComponent.movementVector;
                // Only resolve collision in the opposite direction of movement, so objects don't get accelerated.
                if ((sgn(pair.second.x) > 0 && sgn(collisionComponent.movementVector.x) < 0) ||
                    (sgn(pair.second.x) < 0 && sgn(collisionComponent.movementVector.x) > 0))
                    collisionComponent.movementVector.x += pair.second.x * fabs(collisionComponent.movementVector.x) * (1 - pair.first);

                if ((sgn(pair.second.y) > 0 && sgn(collisionComponent.movementVector.y) < 0) ||
                    (sgn(pair.second.y) < 0 && sgn(collisionComponent.movementVector.y) > 0))
                    collisionComponent.movementVector.y += pair.second.y * fabs(collisionComponent.movementVector.y) * (1 - pair.first);

                engine::Log::d("movX ", collisionComponent.movementVector.x);
                engine::Log::d("movY ", collisionComponent.movementVector.y);
                engine::Log::d("normal ", pair.second.x, "  ", pair.second.y);
                engine::Log::d("frac ", collisionComponent.movementVector.x / old.x, "  ", collisionComponent.movementVector.y / old.y);
            }

            /*if (collisionsX.size() > 0)
                collisionComponent.movementVector.x = collisionComponent.movementVector.x * collisionsX[0];

            if (collisionsY.size() > 0)
                collisionComponent.movementVector.y = collisionComponent.movementVector.y * collisionsY[0];*/

            glm::dvec2 newPosition = positionComponent.position + collisionComponent.movementVector * deltaTime;
            engine::Log::d("pos ", positionComponent.x() + 32, "  ", positionComponent.y() + 32);
            set_position(entity, newPosition.x, newPosition.y);
            collisionComponent.movementVector = {0.0, 0.0};
        }
        collisionObserver->clear();
    }

    bool MovementSystem::Impl::collidingRayAABB(const glm::dvec2 &rayStart, const glm::dvec2 &rayDirection, const engine::Rectangle &aabb, glm::dvec2 &contactPoint, glm::dvec2 &contactNormal, double &contactTime) const
    {
        // This implementation is based on a tutorial by David Barr (javidx9). The whole license (OLC-3) is at the bottom of this file.
        contactPoint = {0.0, 0.0};
        contactNormal = {0.0, 0.0};

        double invDirX = 1.0 / rayDirection.x;
        double invDirY = 1.0 / rayDirection.y;

        // Calculate intersections with rectangle bounding axes
        double tNearX = (aabb.x - rayStart.x) * invDirX;
        double tNearY = (aabb.y - rayStart.y) * invDirY;
        double tFarX = (aabb.x + aabb.width - rayStart.x) * invDirX;
        double tFarY = (aabb.y + aabb.height - rayStart.y) * invDirY;

        // If rayStart is on one of the corners and ray is parallel to one of the axes, then one of the times will be 0/0=NaN (by IEEE standard).
        // In this case we have no collision.
        if (std::isnan(tNearX) || std::isnan(tNearY))
            return false;
        if (std::isnan(tFarX) || std::isnan(tFarY))
            return false;

        // Sort intersection times
        if (tNearX > tFarX)
            std::swap(tNearX, tFarX);
        if (tNearY > tFarY)
            std::swap(tNearY, tFarY);

        // Early rejection
        if (tNearX > tFarY || tNearY > tFarX)
            return false;

        // Closest 'time' will be the first contact
        contactTime = std::max(tNearX, tNearY);

        // Furthest 'time' is contact on opposite side of target
        double contactTimeFar = std::min(tFarX, tFarY);

        // Reject if ray direction is pointing away from object
        if (contactTimeFar < 0 || contactTime > 1.0f)
            return false;

        contactPoint = rayStart + contactTime * rayDirection;

        if (fabs(tNearX - tNearY) < 1e-10)
        {
            // if t_near == t_far, then we have a corner-corner collision.
            // We stop the movement in this case, by having the contact normal point in the opposite moving direction
            contactNormal = -glm::normalize(rayDirection);
        }
        else if (tNearX > tNearY)
            contactNormal = {-sgn(invDirX), 0};
        else if (tNearX < tNearY)
            contactNormal = {0, -sgn(invDirY)};

        return true;
    }

    bool MovementSystem::Impl::collidingMovingAABBMovingAABB(const engine::Rectangle &a, const glm::dvec2 &movementA, const engine::Rectangle &b, const glm::dvec2 &movementB, const double deltaTime, glm::dvec2 &contactPoint, glm::dvec2 &contactNormal, double &contactTime) const
    {
        // Transform vollision into frame of reference of B.
        glm::dvec2 movementAFrameB = (movementA - movementB) * deltaTime;

        // Assume that rectangles don't collide at the beginning, so no movement => no collision
        if (movementAFrameB.x == 0.0 && movementAFrameB.y == 0.0)
            return false;

        engine::Rectangle expandedAABB;
        expandedAABB.x = b.x - a.width / 2;
        expandedAABB.y = b.y - a.height / 2;
        expandedAABB.width = b.width + a.width;
        expandedAABB.height = b.height + a.height;

        glm::dvec2 rayStart = {a.x + a.width / 2, a.y + a.height / 2};
        bool rayCollision = collidingRayAABB(rayStart, movementAFrameB, expandedAABB, contactPoint, contactNormal, contactTime);
        //Tramslate contact time to global reference frame
        contactTime *= (movementA.x - movementB.x) / movementA.x;

        return rayCollision && (contactTime >= 0.0f && contactTime < 1.0f);
    }

    void MovementSystem::Impl::set_position(const entt::entity &entity, const double x, const double y)
    {
        registry.patch<engine::PositionComponent>(entity,
                                                  [=](auto &pos) {
                                                      pos.position.x = x;
                                                      pos.position.y = y;
                                                  });
    }
} // namespace tilegame::worldscene

/*
	One-Size-Fits-All Rectangle Vs Rectangle Collisions
	"Stupid scanners... making me miss at archery..." - javidx9
	License (OLC-3)
	~~~~~~~~~~~~~~~
	Copyright 2018-2020 OneLoneCoder.com
	Redistribution and use in source and binary forms, with or without
	modification, are permitted provided that the following conditions
	are met:
	1. Redistributions or derivations of source code must retain the above
	copyright notice, this list of conditions and the following disclaimer.
	2. Redistributions or derivative works in binary form must reproduce
	the above copyright notice. This list of conditions and the following
	disclaimer must be reproduced in the documentation and/or other
	materials provided with the distribution.
	3. Neither the name of the copyright holder nor the names of its
	contributors may be used to endorse or promote products derived
	from this software without specific prior written permission.
	THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
	"AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
	LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
	A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
	HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
	SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
	LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
	DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
	THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
	(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
	OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
	Relevant Video: https://www.youtube.com/watch?v=8JJ-4JgR7Dg
	Links
	~~~~~
	YouTube:	https://www.youtube.com/javidx9
				https://www.youtube.com/javidx9extra
	Discord:	https://discord.gg/WhwHUMV
	Twitter:	https://www.twitter.com/javidx9
	Twitch:		https://www.twitch.tv/javidx9
	GitHub:		https://www.github.com/onelonecoder
	Patreon:	https://www.patreon.com/javidx9
	Homepage:	https://www.onelonecoder.com
	Community Blog: https://community.onelonecoder.com
	Author
	~~~~~~
	David Barr, aka javidx9, ©OneLoneCoder 2018, 2019, 2020
*/