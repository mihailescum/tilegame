#pragma once

namespace tilegame::components
{
    struct Ordering
    {
        float z;

        float operator()() const { return z; }
    };
} // namespace tilegame
