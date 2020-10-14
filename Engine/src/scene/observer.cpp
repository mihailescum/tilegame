#include "scene/observer.hpp"

namespace engine
{

    Observer::Observer(entt::registry &registry) : registry(registry) {}

    Observer::~Observer()
    {
        this->disconnect();
    }

    void Observer::disconnect()
    {
        this->observer.disconnect();
    }

    void Observer::clear() ENTT_NOEXCEPT
    {
        observer.clear();
    }
} // namespace engine