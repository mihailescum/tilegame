#include ".hpp"

#include "tilegame.hpp"

namespace tilegame::systems
{
    System::System(tilegame::Scene &scene, entt::registry &registry)
        : _scene(scene), _registry(registry)
    {
    }
} // namespace tilegame::systems
