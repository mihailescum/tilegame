#include "core/rectangle.hpp"

namespace engine
{
    const Rectangle Rectangle::EMPTY(glm::vec2(0.0), glm::vec2(0.0));

    bool Rectangle::intersects(const Rectangle &other) const
    {
        return (position.x < other.position.x + other.size.x && position.x + size.x > other.position.x) &&
               (position.y < other.position.y + other.size.y && position.y + size.y > other.position.y);
    }

    bool Rectangle::intersects(const Point &point) const
    {
        return (point.position.x > position.x && point.position.x < position.x + size.x) &&
               (point.position.y > position.y && point.position.y < position.y + size.y);
    }

    bool Rectangle::intersects(const Ray &ray, glm::vec2 &contact_point, glm::vec2 &contact_normal, float &t_hit_near) const
    {
        /* The following Ray-AABB intersection routine is adapted from work by David Barr (javidx9).

            One-Size-Fits-All Rectangle Vs Rectangle Collisions
            "Stupid scanners... making me miss at archery..." - javidx9

            License (OLC-3)
            ~~~~~~~~~~~~~~~

            Copyright 2018-2020 OneLoneCoder.com

            Redistribution and use in source and binary forms, with or without
            modification, are permitted provided that the following conditions
            are met:

            1. Redistributions or derivations of source code must retain the above
            copyright notice, this list of conditions and the following disclaimer.

            2. Redistributions or derivative works in binary form must reproduce
            the above copyright notice. This list of conditions and the following
            disclaimer must be reproduced in the documentation and/or other
            materials provided with the distribution.

            3. Neither the name of the copyright holder nor the names of its
            contributors may be used to endorse or promote products derived
            from this software without specific prior written permission.

            THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
            "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
            LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
            A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
            HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
            SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
            LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
            DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
            THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
            (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
            OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

            Relevant Video: https://www.youtube.com/watch?v=8JJ-4JgR7Dg

            Links
            ~~~~~
            YouTube:	https://www.youtube.com/javidx9
                        https://www.youtube.com/javidx9extra
            Discord:	https://discord.gg/WhwHUMV
            Twitter:	https://www.twitter.com/javidx9
            Twitch:		https://www.twitch.tv/javidx9
            GitHub:		https://www.github.com/onelonecoder
            Patreon:	https://www.patreon.com/javidx9
            Homepage:	https://www.onelonecoder.com

            Community Blog: https://community.onelonecoder.com

            Author
            ~~~~~~
            David Barr, aka javidx9, ©OneLoneCoder 2018, 2019, 2020
        */

        glm::vec2 inv_div = glm::vec2(1.0) / ray.direction;
        glm::vec2 t_near = (position - ray.origin) * inv_div;
        glm::vec2 t_far = (position + size - ray.origin) * inv_div;

        if (std::isnan(t_far.y) || std::isnan(t_far.x))
        {
            return false;
        }
        if (std::isnan(t_near.y) || std::isnan(t_near.x))
        {
            return false;
        }

        if (t_near.x > t_far.x)
        {
            std::swap(t_near.x, t_far.x);
        }
        if (t_near.y > t_far.y)
        {
            std::swap(t_near.y, t_far.y);
        }

        if (t_near.x > t_far.x || t_near.y > t_far.y)
        {
            return false;
        }

        t_hit_near = std::max(t_near.x, t_near.y);
        float t_hit_far = std::min(t_far.x, t_far.y);
        if (t_hit_far < 0)
        {
            return false;
        }

        contact_point = ray.origin + t_hit_near * ray.direction;
        if (t_near.x > t_near.y)
        {
            if (ray.direction.x < 0)
            {
                contact_normal.x = 1.0;
            }
            else
            {
                contact_normal.x = -1.0;
            }
            contact_normal.y = 0.0;
        }
        else if (t_near.x < t_near.y)
        {
            if (ray.direction.y < 0)
            {
                contact_normal.y = 1.0;
            }
            else
            {
                contact_normal.y = -1.0;
            }
            contact_normal.x = 0.0;
        }
        else
        {
            contact_normal.x = 0.0;
            contact_normal.y = 0.0;
        }

        return true;
    }
}
