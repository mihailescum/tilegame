#pragma once

#include <string>

#include <entt/entity/registry.hpp>

namespace tilegame::components
{
    template <class T>
    struct EventListener
    {
        std::function<void(const std::string, const T &, entt::entity)> callback;
        entt::entity source;

        EventListener() : source(entt::null) {}
        EventListener(std::function<void(const std::string, const T &, entt::entity)> callback) : callback(callback), source(entt::null) {}
        EventListener(std::function<void(const std::string, const T &, entt::entity)> callback, entt::entity source) : callback(callback), source(source) {}

        void operator()(const std::string type, const T &event, entt::entity source) const
        {
            if (source == entt::null || source == this->source)
            {
                callback(type, event, source);
            }
        }
    };
} // namespace tilegame::components
