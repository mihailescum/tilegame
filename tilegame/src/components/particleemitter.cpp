#include "particleemitter.hpp"

#include <sstream>

#include "entt/entt.hpp"

#include "entt_sol/bond.hpp"

namespace tilegame::components
{
    std::string ParticleEmitter::to_string() const
    {
        std::stringstream ss;
        ss << "ParticleEmitter";
        return ss.str();
    }

    void ParticleEmitter::register_component(sol::state &lua)
    {
        entt_sol::register_meta_component<ParticleEmitter>();

        lua.new_usertype<ParticleEmitter>(
            "_ParticleEmitter",
            "type_id", &entt::type_hash<ParticleEmitter>::value,
            sol::call_constructor,
            sol::factories(
                []()
                { return ParticleEmitter(); },
                [](int rate, const glm::vec2 &spread_direction, float spread_angle,
                   float speed_min, float speed_max,
                   float lifetime_min, float lifetime_max,
                   float scale_min, float scale_max,
                   const engine::Color &color, const engine::Rectangle &source_rect)
                {
                    return ParticleEmitter(rate, spread_direction, spread_angle,
                                            speed_min, speed_max,
                                            lifetime_min, lifetime_max,
                                            scale_min, scale_max,
                                            color, source_rect);
                }),
            "rate", &ParticleEmitter::rate,
            "spread_direction", &ParticleEmitter::spread_direction,
            "spread_angle", &ParticleEmitter::spread_angle,
            "speed", &ParticleEmitter::speed,
            "lifetime", &ParticleEmitter::lifetime,
            "scale", &ParticleEmitter::scale,
            "color", &ParticleEmitter::color,
            "source_rect", &ParticleEmitter::source_rect,
            sol::meta_function::to_string, &ParticleEmitter::to_string);
    }
} // namespace tilegame::components
