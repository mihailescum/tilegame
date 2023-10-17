#pragma once

#include <string>

#include "entt.hpp"

#include "scene/scene.hpp"
#include "scene/components/tagcomponent.hpp"

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

        template <class Component, class... Args>
        Component &add(Args &&... args) const
        {
            return scene->getRegistry().emplace_or_replace<Component>(this->handle, args...);
        }

        template <class Component>
        Component &add() const
        {
            return scene->getRegistry().emplace_or_replace<Component>(this->handle);
        }

        template <class Component, class... Func>
        Component &patch(Func &&... func) const
        {
            return scene->getRegistry().patch<Component>(this->handle, func...);
        }

        template <class... Components>
        void remove() const
        {
            scene->getRegistry().remove_if_exists<Components...>(this->handle);
        }

        template <class Component>
        Component &get()
        {
            return scene->getRegistry().get<Component>(this->handle);
        }

        template <class... Components>
        bool has() const
        {
            return scene->getRegistry().has<Components...>(this->handle);
        }

        TagComponent &addTag(const std::string &tag)
        {
            return this->add<TagComponent>(tag);
        }

        void removeTag()
        {
            this->remove<TagComponent>();
        }

        template <class... Components>
        bool any() const
        {
            return scene->getRegistry().any<Components...>(this->handle);
        }

        operator bool() const { return this->handle != entt::null; }
        operator entt::entity() const { return this->handle; }
    };
} // namespace engine