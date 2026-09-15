#pragma once

#include <string>
#include <filesystem>

namespace engine
{
    class ResourceManager;

    /**
     * @brief Abstract base for assets managed by ResourceManager (e.g. Shader, Texture2D).
     *
     * Tracks an id, filesystem path and name assigned by the owning
     * ResourceManager, and defines the load/unload lifecycle that concrete
     * resource types must implement.
     */
    class Resource
    {
    protected:
        std::size_t _resource_id;
        std::filesystem::path _resource_path;
        std::string _resource_name;

    public:
        Resource() : _resource_id(0), _resource_path(""), _resource_name("") {}
        virtual ~Resource();

        /// Loads the resource's underlying data; @p args are the variadic arguments passed to ResourceManager::load_resource beyond the name/path.
        virtual bool load_resource(ResourceManager &_resource_manager, va_list args) = 0;
        virtual void unload_resource() = 0;

        void resource_id(std::size_t id);
        std::size_t resource_id() const;

        void resource_path(const std::filesystem::path &path);
        std::filesystem::path resource_path() const;

        void resource_name(const std::string &path);
        std::string resource_name() const;
    };
} // namespace engine
