#pragma once

#include <unordered_map>
#include <memory>
#include <stdexcept>
#include <string>
#include <cstdarg>
#include <filesystem>

#include "glad/glad.h"

#include "resource.hpp"

namespace engine
{
    /**
     * @brief Owns and caches all loaded Resource instances (shaders, textures, ...) keyed by name.
     *
     * Resources are loaded on demand and kept alive for the lifetime of the
     * manager (or until unload_resources() is called), so repeated requests
     * for the same name return the cached instance instead of reloading.
     */
    class ResourceManager
    {
    private:
        // resource storage
        std::unordered_map<std::string, std::unique_ptr<Resource>> _resources;
        std::size_t _last_resource_id;

    public:
        ResourceManager() : _last_resource_id(0) {}
        ResourceManager(const ResourceManager &manager) = delete;
        virtual ~ResourceManager();

        // properly de-allocates all loaded resources
        void unload_resources();

        /**
         * @brief Loads (or returns the already-cached) resource of type T for the given name/path.
         *
         * If no resource is cached under @p name (defaulting to the path's
         * filename when empty), a new T is constructed and
         * Resource::load_resource() is invoked with the trailing variadic
         * arguments; the instance is only cached if loading succeeds.
         * @return Pointer to the cached resource, or nullptr if loading failed.
         */
        template <class T>
        T *load_resource(std::string name, const std::filesystem::path &path, ...)
        {
            (void)static_cast<Resource *>((T *)0);

            if (name.empty())
                name = path.filename();

            if (this->_resources.count(name) == 0)
            {
                std::unique_ptr<T> res = std::make_unique<T>();
                res->resource_id(++_last_resource_id);
                res->resource_path(std::filesystem::absolute(path));
                res->resource_name(name);

                va_list args;
                va_start(args, path);
                bool loading_succeded = res->load_resource(*this, args);
                va_end(args);

                if (loading_succeded)
                {
                    _resources.emplace(name, std::move(res));
                }
            }

            if (_resources.count(name) != 0)
            {
                return static_cast<T *>(_resources[name].get());
            }
            else
            {
                return nullptr;
            }
        }

        /// Returns the cached resource of type T registered under @p name; throws if none exists.
        template <typename T>
        T &get(const std::string name)
        {
            if (_resources.count(name) != 0)
            {
                return static_cast<T &>(*_resources[name].get());
            }
            else
            {
                throw std::runtime_error("Resource not found: " + name);
            }
        }

        /// Takes ownership of an already-constructed resource and registers it under @p name, bypassing the load_resource() path.
        template <typename T>
        T &emplace_resource(const std::string name, std::unique_ptr<T> &res)
        {
            static_assert(std::is_base_of<Resource, T>::value);
            // static_assert(std::is_copy_constructible<T>::value);

            // std::unique_ptr<T> res = std::make_unique<T>(std::move(resource));
            res->resource_id(_resources.size());
            res->resource_name(name);

            _resources.emplace(name, std::move(res));
            return static_cast<T &>(*_resources[name]);
        }
    };
} // namespace engine
