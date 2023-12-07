# Header Files
set (HEADER_PATH include)

file(GLOB HEADER_ENGINE_CORE "${HEADER_PATH}/core/*.hpp")
file(GLOB HEADER_ENGINE_TILEMAP "${HEADER_PATH}/tilemap/*.hpp")
file(GLOB HEADER_ENGINE_GRAPHICS "${HEADER_PATH}/graphics/*.hpp")

# Source Files
set (SOURCES_PATH src)

file(GLOB SOURCES_ENGINE_CORE "${SOURCES_PATH}/core/*.cpp")
file(GLOB SOURCES_ENGINE_TILEMAP "${SOURCES_PATH}/tilemap/*.cpp")
file(GLOB SOURCES_ENGINE_GRAPHICS "${SOURCES_PATH}/graphics/*.cpp")

# Create Library
set (LIBRARY_NAME engine)
add_library(${LIBRARY_NAME} 
        STATIC 
                "${HEADER_PATH}/engine.hpp"

                ${SOURCES_ENGINE_CORE} 
                ${SOURCES_ENGINE_TILEMAP} 
                ${SOURCES_ENGINE_GRAPHICS} 

                ${HEADER_ENGINE_CORE}
                ${HEADER_ENGINE_TILEMAP}
                ${HEADER_ENGINE_GRAPHICS}
)

set_target_properties(
        ${LIBRARY_NAME} 
        PROPERTIES 
        LINKER_LANGUAGE CXX)

# Include Project Header Files
target_include_directories(${LIBRARY_NAME} PUBLIC ${HEADER_PATH})

# Include glad Header Files
target_include_directories(${LIBRARY_NAME} PUBLIC ${PROJECT_SOURCE_DIR}/vendor/glad/include)

# stb_image Header Files
target_include_directories(${LIBRARY_NAME} PUBLIC ${PROJECT_SOURCE_DIR}/vendor/stb_image/include)

# nlohmann/json Header Files
target_include_directories(${LIBRARY_NAME} PUBLIC ${PROJECT_SOURCE_DIR}/vendor/json/include)

# Include Tileson Header Files
target_include_directories(${LIBRARY_NAME} PUBLIC ${PROJECT_SOURCE_DIR}/vendor/tileson)

# Link with Glad
target_link_libraries(${LIBRARY_NAME} PUBLIC glad)

# Link with std::filesystem library
target_link_libraries(${LIBRARY_NAME} PUBLIC stdc++fs)

# GLFW
find_package(glfw3 3.3 REQUIRED)
target_link_libraries(${LIBRARY_NAME} PUBLIC glfw)

# pugixml
target_link_libraries(${LIBRARY_NAME} PUBLIC glad)
