#include "collisiondetection.hpp"

#include <algorithm>
#include <cmath>
#include <stdexcept>

#include <glm/glm.hpp>

#include "math_helper.hpp"
#include "components/inactive.hpp"

namespace tilegame::systems
{
    CollisionDetection::CollisionDetection(tilegame::Scene &scene, entt::registry &registry) : System(scene, registry)
    {
    }

    void CollisionDetection::initialize()
    {
    }

    void CollisionDetection::update(const engine::GameTime &update_time)
    {
        const auto moving_entities = _registry.view<const components::Transform, const components::Collider, components::Movement>(entt::exclude<components::Inactive>);
        const auto tilelayer_entities = _registry.view<const components::Transform, const components::TileLayer, const components::Shape>(entt::exclude<components::Inactive>);

        for (auto &&[entity, transform, collider, movement] : moving_entities.each())
        {
            for (auto &&[tilelayer_entity, tilelayer_transform, tilelayer_data, tilelayer_shape] : tilelayer_entities.each())
            {
                entity_tilelayer_detection(transform, collider, movement, tilelayer_data, tilelayer_transform, tilelayer_shape);
            }
        }
    }

    void CollisionDetection::entity_tilelayer_detection(const components::Transform &entity_transform, const components::Collider &entity_collider, components::Movement &entity_movement, const components::TileLayer &tilelayer, const components::Transform &tilelayer_transform, const components::Shape &tilelayer_shape) const
    {
        // Near phase detection
        std::vector<std::pair<int, float>> found_collisions;

        const auto entity_circle = std::get_if<engine::Circle>(&entity_collider.shape);
        const auto entity_rectangle = std::get_if<engine::Rectangle>(&entity_collider.shape);

        // Broad phase detection: only visit tiles the entity's shape could possibly reach this
        // frame - its current extent unioned with that same extent shifted by its full velocity
        // - padded by one tile so an edge-touching tile is never missed. Scanning every tile of
        // every layer for every moving entity, every frame, does not scale with map size.
        glm::vec2 shape_min, shape_max;
        if (entity_rectangle)
        {
            shape_min = entity_transform.position + entity_rectangle->position;
            shape_max = shape_min + entity_rectangle->dimensions;
        }
        else // entity_circle
        {
            shape_min = entity_transform.position + entity_circle->origin - glm::vec2(entity_circle->radius);
            shape_max = entity_transform.position + entity_circle->origin + glm::vec2(entity_circle->radius);
        }
        const glm::vec2 swept_min = glm::min(shape_min, shape_min + entity_movement.velocity);
        const glm::vec2 swept_max = glm::max(shape_max, shape_max + entity_movement.velocity);

        const glm::vec2 local_min = swept_min - tilelayer_transform.position;
        const glm::vec2 local_max = swept_max - tilelayer_transform.position;
        const glm::ivec2 &tile_dimensions = tilelayer.tile_dimensions;

        const auto shape = std::get_if<engine::Point>(&tilelayer_shape.shape);
        const int tilelayer_width = (int)shape->position.x;
        const int tilelayer_height = (int)shape->position.y;

        const int x_start = std::max(0, static_cast<int>(std::floor(local_min.x / tile_dimensions.x)) - 1);
        const int y_start = std::max(0, static_cast<int>(std::floor(local_min.y / tile_dimensions.y)) - 1);
        const int x_end = std::min(tilelayer_width - 1, static_cast<int>(std::ceil(local_max.x / tile_dimensions.x)) + 1);
        const int y_end = std::min(tilelayer_height - 1, static_cast<int>(std::ceil(local_max.y / tile_dimensions.y)) + 1);

        for (int x = x_start; x <= x_end; x++)
        {
            for (int y = y_start; y <= y_end; y++)
            {
                const auto &tile = tilelayer.tile_data[x + tilelayer_width * y];
                if (!tile.collision_shape)
                {
                    continue;
                }

                const auto tile_circle = std::get_if<engine::Circle>(&(*tile.collision_shape));
                const auto tile_rectangle = std::get_if<engine::Rectangle>(&(*tile.collision_shape));

                if (entity_rectangle && tile_rectangle)
                {
                    const auto a = *entity_rectangle + entity_transform.position;
                    const auto b = *tile_rectangle + tilelayer_transform.position + tile.destination_rect.position;

                    glm::vec2 contact_normal;
                    float contact_time;
                    if (aabb_aabb_detection(a, b, entity_movement.velocity, contact_normal, contact_time))
                    {
                        found_collisions.push_back({x + tilelayer_width * y, contact_time});
                    }
                }
                else if (entity_circle && tile_rectangle)
                {
                    const auto a = *entity_circle + entity_transform.position;
                    const auto b = *tile_rectangle + tilelayer_transform.position + tile.destination_rect.position;

                    glm::vec2 contact_normal;
                    float contact_time;
                    if (circle_aabb_detection(a, b, entity_movement.velocity, contact_normal, contact_time))
                    {
                        found_collisions.push_back({x + tilelayer_width * y, contact_time});
                    }
                }
                else if (entity_rectangle && tile_circle)
                {
                }
                else if (entity_circle && tile_circle)
                {
                    const auto a = *entity_circle + entity_transform.position;
                    const auto b = *tile_circle + tilelayer_transform.position + tile.destination_rect.position;

                    glm::vec2 contact_normal;
                    float contact_time;
                    if (circle_circle_detection(a, b, entity_movement.velocity, contact_normal, contact_time))
                    {
                        found_collisions.push_back({x + tilelayer_width * y, contact_time});
                    }
                }
                else
                {
                    throw std::runtime_error("Unsupported collision shapes");
                }
            }
        }

        std::sort(found_collisions.begin(), found_collisions.end(), [](const auto &a, const auto &b)
                  { return a.second < b.second; });

        for (const auto &[index, ct] : found_collisions)
        {
            const auto &tile = tilelayer.tile_data[index];
            const auto tile_circle = std::get_if<engine::Circle>(&(*tile.collision_shape));
            const auto tile_rectangle = std::get_if<engine::Rectangle>(&(*tile.collision_shape));

            if (entity_rectangle && tile_rectangle)
            {
                const auto a = *entity_rectangle + entity_transform.position;
                const auto b = *tile_rectangle + tilelayer_transform.position + tile.destination_rect.position;

                aabb_aabb_resolution(a, b, entity_movement.velocity);
            }
            else if (entity_circle && tile_rectangle)
            {
                const auto a = *entity_circle + entity_transform.position;
                const auto b = *tile_rectangle + tilelayer_transform.position + tile.destination_rect.position;

                circle_aabb_resolution(a, b, entity_movement.velocity);
            }
            else if (entity_rectangle && tile_circle)
            {
            }
            else if (entity_circle && tile_circle)
            {
                const auto a = *entity_circle + entity_transform.position;
                const auto b = *tile_circle + tilelayer_transform.position + tile.destination_rect.position;

                circle_circle_resolution(a, b, entity_movement.velocity);
            }
            else
            {
                throw std::runtime_error("Unsupported collision shapes");
            }
        }
    }

    bool CollisionDetection::aabb_aabb_detection(const engine::Rectangle &a, const engine::Rectangle &b, const glm::vec2 &a_vel, glm::vec2 &contact_normal, float &contact_time) const
    {
        /*  We implement a sweaped AABB collision detection/resolution system based on
            the video 'Arbitrary Rectangle Collision Detection & Resolution - Complete!' by 'javidx9'
            on YouTube, see https://www.youtube.com/watch?v=8JJ-4JgR7Dg

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

        if (a_vel.x == 0.0 && a_vel.y == 0.0)
        {
            return false;
        }

        engine::Rectangle expanded_rect(b.position - a.dimensions * 0.5f, b.dimensions + a.dimensions);

        engine::Ray ray(a.position + a.dimensions * 0.5f, a_vel);

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

    void CollisionDetection::aabb_aabb_resolution(const engine::Rectangle &a, const engine::Rectangle &b, glm::vec2 &a_vel) const
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

    bool CollisionDetection::circle_aabb_detection(const engine::Circle &a, const engine::Rectangle &b, const glm::vec2 &a_vel, glm::vec2 &contact_normal, float &contact_time) const
    {
        /*  We implement a Circle AABB collision detection/resolution system (not coninuous!) based on
            the video 'Circle Vs Rectangle Collisions (and TransformedView PGEX)' by 'javidx9'
            on YouTube, see https://www.youtube.com/watch?v=D2a5fHX-Qrs

            Easy Circle Vs Rectangle Collision Resolution
            "Everything's just damp, the walls, the roof, everything!" - javidx9

            Video: https://youtu.be/D2a5fHX-Qrs

            License (OLC-3)
            ~~~~~~~~~~~~~~~

            Copyright 2018 - 2021 OneLoneCoder.com

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

            Links
            ~~~~~
            YouTube:	https://www.youtube.com/javidx9
                        https://www.youtube.com/javidx9extra
            Discord:	https://discord.gg/WhwHUMV
            Twitter:	https://www.twitter.com/javidx9
            Twitch:		https://www.twitch.tv/javidx9
            GitHub:		https://www.github.com/onelonecoder
            Homepage:	https://www.onelonecoder.com

            Author
            ~~~~~~
            David Barr, aka javidx9, ©OneLoneCoder 2019, 2020, 2021
        */

        glm::vec2 potential_position = a.origin + a_vel;
        glm::vec2 nearest_point;
        nearest_point.x = std::max(b.position.x, std::min(potential_position.x, b.position.x + b.dimensions.x));
        nearest_point.y = std::max(b.position.y, std::min(potential_position.y, b.position.y + b.dimensions.y));

        glm::vec2 ray_to_neares_point = nearest_point - potential_position;
        float overlap = a.radius - glm::length(ray_to_neares_point);
        if (std::isnan(overlap))
            overlap = 0;

        if (overlap > 0)
        {
            contact_normal = -glm::normalize(ray_to_neares_point);
            contact_time = 1 - overlap / a.radius;
            return true;
        }
        return false;
    }

    void CollisionDetection::circle_aabb_resolution(const engine::Circle &a, const engine::Rectangle &b, glm::vec2 &a_vel) const
    {
        if (a_vel.x == 0.0 && a_vel.y == 0.0)
        {
            return;
        }

        glm::vec2 contact_normal;
        float contact_time;
        if (circle_aabb_detection(a, b, a_vel, contact_normal, contact_time))
        {
            a_vel += contact_normal * (1 - contact_time) * a.radius;
        }
    }

    bool CollisionDetection::circle_circle_detection(const engine::Circle &a, const engine::Circle &b, const glm::vec2 &a_vel, glm::vec2 &contact_normal, float &contact_time) const
    {
        if (a_vel.x == 0.0 && a_vel.y == 0.0)
        {
            return false;
        }

        float R = a.radius + b.radius;
        glm::vec2 distance = a.origin - b.origin;
        float distance_dot_v = glm::dot(distance, a_vel);

        float discriminant = distance_dot_v * distance_dot_v + glm::length2(a_vel) * (R * R - glm::length2(distance));
        if (discriminant < 0)
        {
            return false;
        }

        float discriminant_sqrt = std::sqrt(discriminant);
        float t_near = (-distance_dot_v - discriminant_sqrt) / glm::length2(a_vel);
        float t_far = (-distance_dot_v + discriminant_sqrt) / glm::length2(a_vel);

        if (t_near < 1.0 && t_far > 0.0)
        {
            contact_time = t_near;
            contact_normal = glm::normalize(a.origin + contact_time * a_vel - b.origin);

            return true;
        }
        return false;
    }

    void CollisionDetection::circle_circle_resolution(const engine::Circle &a, const engine::Circle &b, glm::vec2 &a_vel) const
    {
        if (a_vel.x == 0.0 && a_vel.y == 0.0)
        {
            return;
        }

        glm::vec2 contact_normal;
        float contact_time;
        if (circle_circle_detection(a, b, a_vel, contact_normal, contact_time))
        {
            float vel_along_normal = glm::dot(a_vel, contact_normal);
            if (vel_along_normal < 0.0f)
            {
                a_vel -= contact_normal * vel_along_normal * (1 - contact_time);
            }
        }
    }
} // namespace tilegame::systems
