#pragma once

#include "entt.hpp"

namespace engine
{
    class Observer
    {
    private:
        entt::registry &registry;
        entt::observer observer;

    public:
        Observer(entt::registry &registry);
        virtual ~Observer();

        template <typename... Components>
        struct exclude : entt::type_list<Components...>
        {
        };

        /*
        Example use: connectOnUpdate<PositionComponent, CameraComponent>(Observer::exclude<ChildComponent>{});
        Observes if the PositionComponent of an entity that has a CameraComponent but no ChildComponent gets patched/replaced.
        */
        template <class Component, class... IncludeFilters, class... ExcludeFilters>
        void connectOnUpdate(exclude<ExcludeFilters...> = {})
        {
            entt::basic_collector<entt::matcher<entt::type_list<ExcludeFilters...>, entt::type_list<IncludeFilters...>, Component>> collector;
            this->observer.connect(this->registry, collector);
        }

        template <class... Group, class... ExcludeFilters>
        void connectOnGroup(exclude<ExcludeFilters...> exclude = {})
        {
            auto collector = entt::collector.group(exclude);
            this->observer.connect(this->registry, collector);
        }

        void disconnect();

        using iterator = typename entt::sparse_set<entt::entity>::iterator;
        [[nodiscard]] iterator begin() const ENTT_NOEXCEPT { return this->observer.begin(); }
        [[nodiscard]] iterator end() const ENTT_NOEXCEPT { return this->observer.end(); }
        void clear() ENTT_NOEXCEPT;
    };
}; // namespace engine