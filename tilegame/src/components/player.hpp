#pragma once

namespace tilegame::components
{
    struct Player
    {
        const int id;
        int operator()() const { return id; }
    };
} // namespace tilegame
