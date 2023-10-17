#pragma once

#include <string>
#include <glm/glm.hpp>

namespace engine
{
    struct TagComponent
    {
        std::string tag;

        TagComponent() = default;
        TagComponent(const TagComponent &) = default;
        TagComponent(const std::string &tag)
            : tag(tag) {}

        operator std::string &() { return this->tag; }
        operator const std::string &() const { return this->tag; }
    };
} // namespace engine