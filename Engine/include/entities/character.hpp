#ifndef __ENGINE_CHARACTER_H__
#define __ENGINE_CHARACTER_H__

#include "entities/entity.hpp"

namespace engine
{
    class Character : public Entity
    {
    private:
        int id;
        int sprite_x;
        int sprite_y;
        int sprite_width;
        int sprite_height;

    public:
    };
} // namespace engine

#endif