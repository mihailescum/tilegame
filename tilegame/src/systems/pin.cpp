#include "pin.hpp"

#include "components/transform.hpp"
#include "components/pin.hpp"

namespace tilegame::systems
{
    Pin::Pin(tilegame::Scene &scene, entt::registry &registry) : System(scene, registry)
    {
    }

    void Pin::initialize()
    {
    }

    void Pin::update(const engine::GameTime &update_time)
    {
        const auto view = _registry.view<const components::Transform, const components::Pin>(entt::exclude<components::Inactive>);
        for (const auto &entity : view)
        {
            const auto &pin = view.get<const components::Pin>(entity);
            const auto &parent_transform = _registry.get<const components::Transform>(pin());
            _registry.patch<components::Transform>(entity,
                                                   [&parent_transform](auto &transform)
                                                   {
                                                       transform.position = parent_transform.position;
                                                   });
        }
    }
} // namespace tilegame::systems
