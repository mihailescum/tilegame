#pragma once

#include <tuple>
#include <array>

#include <glm/glm.hpp>

#include "core/texture2d.hpp"

namespace engine
{
    template <unsigned int N>
    class Texture2DContainer
    {
    private:
        std::array<engine::Texture2D, N> _data;

    public:
        typedef std::array<typename engine::Texture2D::native_type, N> native_type;

        const glm::ivec2 &dimensions() const { return _data.at(0).dimensions(); }
        const auto &at(std::size_t i) const { return _data.at(i); }
        auto &at(std::size_t i) { return _data.at(i); }
    };
} // namespace engine
