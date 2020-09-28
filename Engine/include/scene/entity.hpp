#pragma once

#include <string>

#include "entt.hpp"

#include "scene/scene.hpp"

namespace engine
{
    class Entity
    {
    private:
        Scene *scene;
        entt::entity handle;

    public:
        Entity() = default;
        Entity(entt::entity handle, Scene *scene);
        Entity(const Entity &other) = default;

        template<class Component, class... Args>
        Component &add(Args&&... args) const {
            return scene->registry.emplace_or_replace<Component>(this->handle, args...);
        }

        template<class... Components>
        void remove() const {
            scene->registry.remove_if_exists<Components...>(this->handle);
        }

        template<class Component>
        Component &get() {
            return scene->registry.get<Component>(this->handle);
        }

        template<class... Components>
        bool has() const {
            return scene->registry.has<Components...>(this->handle);
        }

        template<class... Components>
        bool any() const {
            return scene->registry.any<Components...>(this->handle);
        }

        void addTag(const std::string &tag);
        void removeTag();

		operator bool() const { return this->handle != entt::null; }
    };
} // namespace engine