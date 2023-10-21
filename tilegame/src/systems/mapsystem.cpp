#include "mapsystem.hpp"

#include "components/renderable2d.hpp"
#include "components/worldtransform.hpp"

namespace tilegame::systems
{
    MapSystem::MapSystem(engine::Scene &scene, entt::registry &registry) : System(scene, registry)
    {
    }
}