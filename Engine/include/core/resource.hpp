#pragma once

#include <string>
#include <filesystem>

namespace engine
{
    class ResourceManager;

    class Resource
    {
    protected:
        std::size_t resourceId;
        std::filesystem::path resourcePath;
        std::string resourceName;

    public:
        Resource() {}
        virtual ~Resource();

        virtual bool loadResource(ResourceManager &resourceManager, va_list args) = 0;
        virtual void unloadResource() = 0;

        void setResourceId(std::size_t id);
        std::size_t getResourceId() const;

        void setResourcePath(const std::filesystem::path &path);
        std::filesystem::path getResourcePath() const;

        void setResourceName(const std::string &path);
        std::string getResourceName() const;
    };
} // namespace engine
