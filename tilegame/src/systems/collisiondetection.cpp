#include "collisiondetection.hpp"

namespace tilegame::systems
{
    /*  We implement a sweaped AABB collision detection/resolution system based on
        the video 'Arbitrary Rectangle Collision Detection & Resolution - Complete!' by 'javidx9'
        on YouTube, see https://www.youtube.com/watch?v=8JJ-4JgR7Dg 
    */

    CollisionDetection::CollisionDetection(tilegame::Scene &scene, entt::registry &registry) : System(scene, registry)
    {
    }

    void CollisionDetection::initialize()
    {
    }

    void CollisionDetection::update(const engine::GameTime &update_time)
    {
    }
} // namespace tilegame::systems
