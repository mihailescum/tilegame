#pragma once

#include <string>

#include "engine.hpp"

#include "system.hpp"

namespace tilegame::systems
{
    class Daytime : public System
    {
    private:
        engine::Shader *_shader;

    public:
        Daytime(tilegame::Scene &scene, entt::registry &registry);

        void initialize();
        void load_content();
        void update(const engine::GameTime &update_time);
    };
} // namespace tilegame