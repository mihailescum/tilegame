#pragma once

#include <tuple>
#include <array>
#include <initializer_list>

#include <glm/glm.hpp>

#include "core/texture2d.hpp"

namespace engine
{
    template <unsigned int N>
    class Texture2DContainer
    {
    private:
        typedef std::array<typename engine::Texture2D::native_type, N> _native_type;
        std::array<const engine::Texture2D *, N> _data;
        _native_type _gl_textures;

    public:
        typedef _native_type native_type;

        Texture2DContainer()
        {
            for (unsigned int i = 0; i < N; i++)
            {
                _data[i] = nullptr;
                _gl_textures[i] = 0;
            }
        }

        Texture2DContainer(std::initializer_list<const Texture2D *> l)
        {
            set(l);
        }

        void set(std::initializer_list<const Texture2D *> l)
        {
            assert(l.size() == N);

            auto li = l.begin();
            for (unsigned int i = 0; i < N; ++i, ++li)
            {
                _data[i] = *li;
                if (*li)
                {
                    _gl_textures[i] = **li;
                }
                else
                {
                    _gl_textures[i] = 0;
                }
            }
        }

        static void use(const native_type &data, GLenum unit)
        {
            for (unsigned int i = 0; i < N; i++)
            {
                Texture2D::use(data[i], i);
            }
        }

        operator native_type() const { return _gl_textures; }
        operator bool() const { return static_cast<bool>(_data[0]); }
        const glm::ivec2 &dimensions() const { return _data.at(0)->dimensions(); }
        const auto &at(std::size_t i) const { return _data.at(i); }
    };
} // namespace engine
