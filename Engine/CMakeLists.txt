# Source Files
set (SOURCES_PATH src)

set (SOURCES_ENGINE_CORE
        ${SOURCES_PATH}/core/game.cpp
        ${SOURCES_PATH}/core/window.cpp
        ${SOURCES_PATH}/core/shader.cpp
        ${SOURCES_PATH}/core/texture2D.cpp
        ${SOURCES_PATH}/core/log.cpp
        ${SOURCES_PATH}/core/graphicsdevice.cpp
        ${SOURCES_PATH}/core/viewport.cpp
        ${SOURCES_PATH}/core/spritebatch.cpp
        ${SOURCES_PATH}/core/color.cpp
        ${SOURCES_PATH}/core/resource.cpp
        ${SOURCES_PATH}/core/resourcemanager.cpp
        ${SOURCES_PATH}/core/spritesheet.cpp
        ${SOURCES_PATH}/core/spriteinfo.cpp
)

set (SOURCES_ENGINE_MAP
        ${SOURCES_PATH}/world/map.cpp
        ${SOURCES_PATH}/world/tilelayer.cpp
        ${SOURCES_PATH}/world/tileset.cpp
        ${SOURCES_PATH}/world/tile.cpp
        ${SOURCES_PATH}/world/tilelayercomponent.cpp
        ${SOURCES_PATH}/world/objectlayer.cpp
        ${SOURCES_PATH}/world/character.cpp
)

set (SOURCES_ENGINE_SCENE
        ${SOURCES_PATH}/scene/scene.cpp
        ${SOURCES_PATH}/scene/entity.cpp
        ${SOURCES_PATH}/scene/observer.cpp
        ${SOURCES_PATH}/scene/components/positioncomponent.cpp
        ${SOURCES_PATH}/scene/components/cameracomponent.cpp
        ${SOURCES_PATH}/scene/components/inputcomponent.cpp
        ${SOURCES_PATH}/scene/components/spritecomponent.cpp
        ${SOURCES_PATH}/scene/components/spritesheetcomponent.cpp
)

set (SOURCES_ENGINE_SPRITES
        ${SOURCES_PATH}/sprites/sprite.cpp
)

# Header Files
set (HEADER_PATH include)

set (HEADER_ENGINE_CORE
        ${HEADER_PATH}/core/game.hpp
        ${HEADER_PATH}/core/window.hpp
        ${HEADER_PATH}/core/shader.hpp
        ${HEADER_PATH}/core/texture2D.hpp
        ${HEADER_PATH}/core/log.hpp
        ${HEADER_PATH}/core/graphicsdevice.hpp
        ${HEADER_PATH}/core/viewport.hpp
        ${HEADER_PATH}/core/rectangle.hpp
        ${HEADER_PATH}/core/spritebatch.hpp
        ${HEADER_PATH}/core/color.hpp
        ${HEADER_PATH}/core/resource.hpp
        ${HEADER_PATH}/core/resourcemanager.hpp
        ${HEADER_PATH}/core/spritesheet.hpp
        ${HEADER_PATH}/core/spriteinfo.hpp
)

set (HEADER_ENGINE_MAP
        ${HEADER_PATH}/world/map.hpp
        ${HEADER_PATH}/world/tilelayer.hpp
        ${HEADER_PATH}/world/tileset.hpp
        ${HEADER_PATH}/world/tile.hpp
        ${HEADER_PATH}/world/tilelayercomponent.hpp
        ${HEADER_PATH}/world/tilesetcomponent.hpp
        ${HEADER_PATH}/world/objectlayer.hpp
        ${HEADER_PATH}/world/character.hpp
)

set (HEADER_ENGINE_SCENE
        ${HEADER_PATH}/scene/scene.hpp
        ${HEADER_PATH}/scene/entity.hpp
        ${HEADER_PATH}/scene/observer.hpp
        ${HEADER_PATH}/scene/components/childcomponent.hpp
        ${HEADER_PATH}/scene/components/positioncomponent.hpp
        ${HEADER_PATH}/scene/components/movecomponent.hpp
        ${HEADER_PATH}/scene/components/cameracomponent.hpp
        ${HEADER_PATH}/scene/components/tagcomponent.hpp
        ${HEADER_PATH}/scene/components/rendercomponent.hpp
        ${HEADER_PATH}/scene/components/inputcomponent.hpp
        ${HEADER_PATH}/scene/components/spritecomponent.hpp
        ${HEADER_PATH}/scene/components/spritesheetcomponent.hpp
)

set (HEADER_ENGINE_SPRITES
        ${HEADER_PATH}/sprites/sprite.hpp
)

# Create Library
set (LIBRARY_NAME engine)
add_library(${LIBRARY_NAME} 
        STATIC 
                ${HEADER_PATH}/engine.hpp

                ${SOURCES_ENGINE_CORE} 
                ${SOURCES_ENGINE_MAP}
                ${SOURCES_ENGINE_SCENE}
                ${SOURCES_ENGINE_SPRITES}

                ${HEADER_ENGINE_CORE}
                ${HEADER_ENGINE_MAP}
                ${HEADER_ENGINE_SCENE}
                ${HEADER_ENGINE_SPRITES}
)

set_target_properties(
        ${LIBRARY_NAME} 
        PROPERTIES 
        LINKER_LANGUAGE CXX)

# Include Project Header Files
target_include_directories(${LIBRARY_NAME} PUBLIC ${HEADER_PATH})

# Include glad Header Files
target_include_directories(${LIBRARY_NAME} PUBLIC ${PROJECT_SOURCE_DIR}/ExternalLibraries/glad/include)

#EnTT Header Files
target_include_directories(${LIBRARY_NAME} PUBLIC ${PROJECT_SOURCE_DIR}/ExternalLibraries/entt/include)

# stb_image Header Files
target_include_directories(${LIBRARY_NAME} PUBLIC ${PROJECT_SOURCE_DIR}/ExternalLibraries/stb_image/include)

# nlohmann/json Header Files
target_include_directories(${LIBRARY_NAME} PUBLIC ${PROJECT_SOURCE_DIR}/ExternalLibraries/json/include)

# Link with Glad
target_link_libraries(${LIBRARY_NAME} PUBLIC glad)

# Link with std::filesystem library
target_link_libraries(${LIBRARY_NAME} PUBLIC stdc++fs)

#GLFW
find_package(glfw3 3.3 REQUIRED)
target_link_libraries(${LIBRARY_NAME} PUBLIC glfw)

#tinyxml2
find_package(tinyxml2)
target_link_libraries(${LIBRARY_NAME} PUBLIC tinyxml2)
