#include "collisiondetection.hpp"

#include <glm/glm.hpp>

#include "components/inactive.hpp"

namespace tilegame::systems
{
    /*  We implement a sweaped AABB collision detection/resolution system based on
        the video 'Arbitrary Rectangle Collision Detection & Resolution - Complete!' by 'javidx9'
        on YouTube, see https://www.youtube.com/watch?v=8JJ-4JgR7Dg

        The following Ray-AABB intersection routine is adapted from work by David Barr (javidx9).

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

    CollisionDetection::CollisionDetection(tilegame::Scene &scene, entt::registry &registry) : System(scene, registry)
    {
    }

    void CollisionDetection::initialize()
    {
    }

    void CollisionDetection::update(const engine::GameTime &update_time)
    {
        const auto moving_entities = _registry.view<const components::Transform, const components::Collider, components::Movement>(entt::exclude<components::Inactive>);
        const auto tilelayer_entities = _registry.view<const components::Transform, const components::TileLayer>(entt::exclude<components::Inactive>);

        for (auto &&[entity, transform, collider, movement] : moving_entities.each())
        {
            for (auto &&[tilelayer_entity, tilelayer_transform, tilelayer_data] : tilelayer_entities.each())
            {
                entity_tilelayer_detection(transform, collider, movement, tilelayer_data, tilelayer_transform);
            }
        }
    }

    void CollisionDetection::entity_tilelayer_detection(const components::Transform &entity_transform, const components::Collider &entity_collider, components::Movement &entity_movement, const components::TileLayer &tilelayer, const components::Transform &tilelayer_transform)
    {
        // Broad phase detection

        // Near phase detection
        std::vector<std::pair<int, float>> found_collisions;

        for (int x = 0; x < tilelayer().size().x; x++)
        {
            for (int y = 0; y < tilelayer().size().y; y++)
            {
                const auto &tile = tilelayer.tile_data[tilelayer().index(x, y)];
                if (!tile.collision_shape)
                {
                    continue;
                }

                if (const auto entity_circle = dynamic_cast<const engine::Circle *>(entity_collider.shape.get()))
                {
                    if (const auto tile_circle = dynamic_cast<const engine::Circle *>(tile.collision_shape))
                    {
                    }
                    else if (const auto tile_rectangle = dynamic_cast<const engine::Rectangle *>(tile.collision_shape))
                    {
                        const auto a = *entity_circle + entity_transform.position;
                        const auto b = *tile_rectangle + tilelayer_transform.position + tile.destination_rect.position;
                        // circle_aabb_detection(a, b, entity_movement.velocity);
                    }
                    else
                    {
                        throw "Unsupported collision shape for tile";
                    }
                }
                else if (auto entity_rectangle = dynamic_cast<const engine::Rectangle *>(entity_collider.shape.get()))
                {
                    if (const auto tile_circle = dynamic_cast<const engine::Circle *>(tile.collision_shape))
                    {
                    }
                    else if (const auto tile_rectangle = dynamic_cast<const engine::Rectangle *>(tile.collision_shape))
                    {
                        const auto a = *entity_rectangle + entity_transform.position;
                        const auto b = *tile_rectangle + tilelayer_transform.position + tile.destination_rect.position;

                        glm::vec2 contact_normal;
                        float contact_time;
                        if (aabb_aabb_detection(a, b, entity_movement.velocity, contact_normal, contact_time))
                        {
                            found_collisions.push_back({tilelayer().index(x, y), contact_time});
                        }
                    }
                    else
                    {
                        throw "Unsupported collision shape for tile";
                    }
                }
                else
                {
                    throw "Unsupported collision shape for entity";
                }
            }
        }

        std::sort(found_collisions.begin(), found_collisions.end(), [](const auto &a, const auto &b)
                  { return a.second < b.second; });

        for (const auto &[index, ct] : found_collisions)
        {
            const auto &tile = tilelayer.tile_data[index];

            if (const auto entity_circle = dynamic_cast<const engine::Circle *>(entity_collider.shape.get()))
            {
                if (const auto tile_circle = dynamic_cast<const engine::Circle *>(tile.collision_shape))
                {
                }
                else if (const auto tile_rectangle = dynamic_cast<const engine::Rectangle *>(tile.collision_shape))
                {
                }
                else
                {
                    throw "Unsupported collision shape for tile";
                }
            }
            else if (auto entity_rectangle = dynamic_cast<const engine::Rectangle *>(entity_collider.shape.get()))
            {
                if (const auto tile_circle = dynamic_cast<const engine::Circle *>(tile.collision_shape))
                {
                }
                else if (const auto tile_rectangle = dynamic_cast<const engine::Rectangle *>(tile.collision_shape))
                {
                    const auto a = *entity_rectangle + entity_transform.position;
                    const auto b = *tile_rectangle + tilelayer_transform.position + tile.destination_rect.position;

                    aabb_aabb_resolution(a, b, entity_movement.velocity);
                }
                else
                {
                    throw "Unsupported collision shape for tile";
                }
            }
            else
            {
                throw "Unsupported collision shape for entity";
            }
        }
    }

    bool CollisionDetection::aabb_aabb_detection(const engine::Rectangle &a, const engine::Rectangle &b, const glm::vec2 &a_vel, glm::vec2 &contact_normal, float &contact_time)
    {
        if (a_vel.x == 0.0 && a_vel.y == 0.0)
        {
            return false;
        }

        engine::Rectangle expanded_rect(b.position - a.size * 0.5f, b.size + a.size);

        engine::Ray ray(a.position + a.size * 0.5f, a_vel);

        glm::vec2 contact_point;
        if (expanded_rect.intersects(ray, contact_point, contact_normal, contact_time))
        {
            return (contact_time >= 0.0f && contact_time < 1.0f);
        }
        else
        {
            return false;
        }
    }

    void CollisionDetection::aabb_aabb_resolution(const engine::Rectangle &a, const engine::Rectangle &b, glm::vec2 &a_vel)
    {
        if (a_vel.x == 0.0 && a_vel.y == 0.0)
        {
            return;
        }

        glm::vec2 contact_normal;
        float contact_time;
        if (aabb_aabb_detection(a, b, a_vel, contact_normal, contact_time))
        {
            a_vel.x += contact_normal.x * std::abs(a_vel.x) * (1 - contact_time);
            a_vel.y += contact_normal.y * std::abs(a_vel.y) * (1 - contact_time);
        }
    }

    void CollisionDetection::circle_aabb_detection(const engine::Circle &a, const engine::Rectangle &b, glm::vec2 &a_vel)
    {
    }
} // namespace tilegame::systems
