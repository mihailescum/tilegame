#include "system.hpp"

#include "tilegame.hpp"

namespace tilegame::systems
{
    System::System(tilegame::Scene &scene, entt::registry &registry)
        : _scene(scene), _registry(registry)
    {
    }

    System::~System() {}
} // namespace tilegame::systems
