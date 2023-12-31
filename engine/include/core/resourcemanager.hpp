#pragma once

#include <unordered_map>
#include <memory>
#include <string>
#include <cstdarg>
#include <filesystem>

#include "glad/glad.h"

#include "resource.hpp"

namespace engine
{
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

        template <typename T>
        T &get(const std::string name)
        {
            if (_resources.count(name) != 0)
            {
                return static_cast<T &>(*_resources[name].get());
            }
            else
            {
                throw "Resource not found";
            }
        }

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
