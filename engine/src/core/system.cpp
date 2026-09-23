#include "core/system.hpp"

namespace engine
{
    System::System(Scene &scene, entt::registry &registry)
        : _scene(scene), _registry(registry)
    {
    }

    System::~System() {}
} // namespace tilegame::systems
