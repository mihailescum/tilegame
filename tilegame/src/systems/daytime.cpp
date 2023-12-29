#include "daytime.hpp"

namespace tilegame::systems
{
    Daytime::Daytime(tilegame::Scene &scene, entt::registry &registry) : System(scene, registry)
    {
    }

    void Daytime::initialize()
    {
    }

    void Daytime::load_content()
    {
        _shader = _scene.game().resource_manager().load_resource<engine::Shader>(
            "daytime_shader",
            "content/shaders/daytime",
            "content/shaders/daytime.vert", "", "content/shaders/daytime.frag");
        _scene.game().postprocessor().shaders().push_back(_shader);

        _shader->use();
        _shader->set("scene", 0);
        _shader->set("tint_color", glm::vec4(1.0, 0.8, 0.8, 1.0));
    }

    void Daytime::update(const engine::GameTime &update_time)
    {
    }
} // namespace tilegame::systems
