#pragma once

namespace engine
{
    struct Shape
    {
        virtual ~Shape() = 0;
        virtual Shape *clone() const = 0;
    };
} // namespace engine
