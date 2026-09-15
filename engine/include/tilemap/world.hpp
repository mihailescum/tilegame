#pragma once

#include <string>
#include <unordered_map>

#include <glm/glm.hpp>

#include "core/resource.hpp"
#include "tilemap/tilemap.hpp"

namespace engine::tilemap
{
    /**
     * @brief A Resource that loads a Tiled world (.world) file: a set of TileMap
     * resources positioned relative to one another in world space. Map file paths
     * are resolved relative to the world file's own directory, and each referenced
     * map is loaded through the same ResourceManager (so a map used by multiple
     * worlds, or loaded elsewhere, is only parsed once). Maps are keyed by name
     * (their file's stem, e.g. "map1" for "map1.tmj").
     */
    class World : public engine::Resource
    {
    public:
        struct MapEntry
        {
            const TileMap *map;
            glm::vec2 position; ///< World-space offset (in pixels) at which this map's origin should be rendered.
            glm::vec2 size;     ///< The map's pixel-space dimensions, as authored in the world file.
        };

    private:
        std::unordered_map<std::string, MapEntry> _maps;

    public:
        virtual bool load_resource(ResourceManager &resource_manager, va_list args) override;
        virtual void unload_resource() override;

        const std::unordered_map<std::string, MapEntry> &maps() const { return _maps; }

        /** @brief Converts coordinates relative to the named map's origin into world-space (global) coordinates. Throws if no map with that name was loaded. */
        glm::vec2 to_global(const std::string &map_name, const glm::vec2 &relative_position) const;

        /** @brief Returns the name of the map whose bounds contain @p global_position, or an empty string if none does. */
        std::string map_at(const glm::vec2 &global_position) const;
    };
} // namespace engine::tilemap
