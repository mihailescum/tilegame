#include "system.hpp"

#include "tilegame.hpp"

namespace tilegame::systems
{
    System::System(engine::Scene &scene, entt::registry &registry, tilegame::SceneGraphNode &scene_graph_root)
        : _scene(scene), _registry(registry), _scene_graph_root(scene_graph_root)
    {
    }
} // namespace tilegame::systems
