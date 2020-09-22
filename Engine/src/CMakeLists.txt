# Source Files
set (SOURCES_ENGINE_CORE
        core/game.cpp
        core/window.cpp
        core/shader.cpp
        core/texture2D.cpp
        core/log.cpp
        core/graphicsdevice.cpp
        core/viewport.cpp
        core/spritebatch.cpp
        core/color.cpp
)

set (SOURCES_ENGINE_MAP
        map/map.cpp
        map/maprenderer.cpp
        map/tile.cpp
        map/tileset.cpp
)

# Header Files
set (HEADER_PATH ${PROJECT_SOURCE_DIR}/Engine/include)

set (HEADER_ENGINE_CORE
        ${HEADER_PATH}/core/game.hpp
        ${HEADER_PATH}/core/window.hpp
        ${HEADER_PATH}/core/shader.hpp
        ${HEADER_PATH}/core/texture2D.hpp
        ${HEADER_PATH}/core/stb_image.hpp
        ${HEADER_PATH}/core/log.hpp
        ${HEADER_PATH}/core/graphicsdevice.hpp
        ${HEADER_PATH}/core/viewport.hpp
        ${HEADER_PATH}/core/rectangle.hpp
        ${HEADER_PATH}/core/spritebatch.hpp
        ${HEADER_PATH}/core/color.hpp
)

set (HEADER_ENGINE_MAP
        ${HEADER_PATH}/map/map.hpp
        ${HEADER_PATH}/map/maprenderer.hpp
        ${HEADER_PATH}/map/tile.hpp
        ${HEADER_PATH}/map/tileset.hpp
)

# Create Library
set (LIBRARY_NAME engine)
add_library(${LIBRARY_NAME} 
        STATIC 
                ${HEADER_PATH}/engine.hpp
                ${SOURCES_ENGINE_CORE} 
                ${SOURCES_ENGINE_MAP}
                ${HEADER_ENGINE_CORE}
                ${HEADER_ENGINE_MAP}
)

set_target_properties(
        ${LIBRARY_NAME} 
        PROPERTIES 
        LINKER_LANGUAGE CXX)

# Include Project Header Files
target_include_directories(${LIBRARY_NAME} PUBLIC ${HEADER_PATH})

# Include glm Header Files
target_include_directories(${LIBRARY_NAME} PUBLIC ${PROJECT_SOURCE_DIR}/ExternalLibraries/glm/include)

# Include glad Header Files
target_include_directories(${LIBRARY_NAME} PUBLIC ${PROJECT_SOURCE_DIR}/ExternalLibraries/glad/include)

# Link with Glad
target_link_libraries(${LIBRARY_NAME} PUBLIC glad)

#GLFW
find_package(glfw3 3.3 REQUIRED)
target_link_libraries(${LIBRARY_NAME} PUBLIC glfw)
