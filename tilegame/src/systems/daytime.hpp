#pragma once

#include <string>
#include <vector>

#include "engine.hpp"

#include "system.hpp"

namespace tilegame::systems
{
    struct TimeOfDayMark
    {
        int start;
        engine::Color tint_color;

        TimeOfDayMark(int start, const engine::Color &tint_color) : start(start), tint_color(tint_color) {}
    };

    class Daytime : public System
    {
    private:
        static const int DAY_DURATION = 86400;

        engine::Shader *_shader;

        std::vector<TimeOfDayMark> _times_of_day;
        std::vector<TimeOfDayMark>::iterator _now_mark;
        int _now;        // Seconds from midnight
        double _speedup; // How many ingame seconds pass in one real world second

    public:
        Daytime(tilegame::Scene &scene, entt::registry &registry);

        void initialize();
        void load_content();
        void update(const engine::GameTime &update_time);
    };
} // namespace tilegame