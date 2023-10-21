#pragma once

#include <string>
#include <filesystem>

namespace engine
{
    class ResourceManager;

    class Resource
    {
    protected:
        std::size_t _resource_id;
        std::filesystem::path _resource_path;
        std::string _resource_name;

    public:
        Resource() {}
        virtual ~Resource();

        virtual bool load_resource(ResourceManager &_resource_manager, va_list args) = 0;
        virtual void unload_resource() = 0;

        void set_resource_id(std::size_t id);
        std::size_t get_resource_id() const;

        void set_resource_path(const std::filesystem::path &path);
        std::filesystem::path get_resource_path() const;

        void set_resource_name(const std::string &path);
        std::string get_resource_name() const;
    };
} // namespace engine
