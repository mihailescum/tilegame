#include "system.hpp"

#include "tilegame.hpp"

namespace tilegame::systems
{
    System::System(engine::Scene &scene, entt::registry &registry) : _scene(scene), _registry(registry)
    {
    }
} // namespace tilegame::systems
