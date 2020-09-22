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
)

set (SOURCES_ENGINE_MAP
        ${SOURCES_PATH}/map/map.cpp
        ${SOURCES_PATH}/map/maprenderer.cpp
        ${SOURCES_PATH}/map/tile.cpp
        ${SOURCES_PATH}/map/tileset.cpp
)

# Header Files
set (HEADER_PATH include)

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

# Include tinyxml2 Header Files
target_include_directories(${LIBRARY_NAME} PUBLIC ${PROJECT_SOURCE_DIR}/ExternalLibraries/tinyxml2/include)

# Link with Glad
target_link_libraries(${LIBRARY_NAME} PUBLIC glad)

# Link with tinyxml2
target_link_libraries(${LIBRARY_NAME} PUBLIC tinyxml2)

#GLFW
find_package(glfw3 3.3 REQUIRED)
target_link_libraries(${LIBRARY_NAME} PUBLIC glfw)
