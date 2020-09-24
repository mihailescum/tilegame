#ifndef __ENGINE_RESOURCE_H__
#define __ENGINE_RESOURCE_H__

#include <string>

namespace engine
{
    class ResourceManager;

    class Resource
    {
    protected:
        unsigned resourceId;
        std::string resourcePath;
        std::string resourceName;

    public:
        Resource() {}
        virtual ~Resource();

        virtual bool loadResource(ResourceManager &resourceManager, const std::string &filename, va_list args) = 0;
        virtual void unloadResource() = 0;

        void setResourceId(unsigned id);
        unsigned getResourceId() const;

        void setResourcePath(const std::string &path);
        std::string getResourcePath() const;

        void setResourceName(const std::string &path);
        std::string getResourceName() const;
    };
} // namespace engine

#endif