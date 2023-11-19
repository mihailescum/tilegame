#pragma once

#include <entt/entity/registry.hpp>

namespace tilegame::components
{
    template <class T>
    struct EventListener
    {
        std::function<void(const std::string, const T &)> callback;
        entt::entity source;

        EventListener() : source(entt::null) {}
        EventListener(std::function<void(const std::string, const T &)> callback) : callback(callback), source(entt::null) {}
        EventListener(std::function<void(const std::string, const T &)> callback, entt::entity source) : callback(callback), source(source) {}
    };
} // namespace tilegame::components
