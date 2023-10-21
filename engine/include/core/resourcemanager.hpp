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
        std::unordered_map<std::string, std::unique_ptr<Resource>> resources;

    public:
        ResourceManager() {}
        ResourceManager(const ResourceManager &manager) = delete;
        virtual ~ResourceManager();

        // properly de-allocates all loaded resources
        void unloadResources();

        Resource *getResource(const std::string &name) const;

        template <class T>
        T *loadResource(std::string name, const std::filesystem::path &path, ...)
        {
            (void)static_cast<Resource *>((T *)0);

            if (name.empty())
                name = path.filename();

            if (this->resources.count(name) == 0)
            {
                std::unique_ptr<T> res = std::make_unique<T>();
                res->setResourceId(this->resources.size());
                res->setResourcePath(std::filesystem::absolute(path));
                res->setResourceName(name);

                va_list args;
                va_start(args, path);
                bool loadingSucceded = res->loadResource(*this, args);
                va_end(args);

                if (loadingSucceded)
                    this->resources.emplace(name, std::move(res));
            }

            if (this->resources.count(name) != 0)
                return (T *)this->resources[name].get();
            else
                return nullptr;
        }
    };
} // namespace engine
