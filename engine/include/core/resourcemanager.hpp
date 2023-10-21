#pragma once

#include <unordered_map>
#include <memory>
#include <string>
#include <cstdarg>
#include <filesystem>

#include <glad/glad.h>

#include "resource.hpp"

namespace engine
{
    class ResourceManager
    {
    private:
        // resource storage
        std::unordered_map<std::string, std::unique_ptr<Resource>> _resources;

    public:
        ResourceManager() {}
        ResourceManager(const ResourceManager &manager) = delete;
        virtual ~ResourceManager();

        // properly de-allocates all loaded resources
        void unload_resources();

        Resource *get_resource(const std::string &name) const;

        template <class T>
        T *load_resource(std::string name, const std::filesystem::path &path, ...)
        {
            (void)static_cast<Resource *>((T *)0);

            if (name.empty())
                name = path.filename();

            if (this->_resources.count(name) == 0)
            {
                std::unique_ptr<T> res = std::make_unique<T>();
                res->set_resource_id(this->_resources.size());
                res->set_resource_path(std::filesystem::absolute(path));
                res->set_resource_name(name);

                va_list args;
                va_start(args, path);
                bool loading_succeded = res->load_resource(*this, args);
                va_end(args);

                if (loading_succeded)
                    this->_resources.emplace(name, std::move(res));
            }

            if (this->_resources.count(name) != 0)
            {
                return (T *)this->_resources[name].get();
            }
            else
            {
                return nullptr;
            }
        }
    };
} // namespace engine
