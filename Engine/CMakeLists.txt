# Header Files
set (HEADER_PATH include)

file(GLOB HEADER_ENGINE_CORE "${HEADER_PATH}/core/*.hpp")
file(GLOB HEADER_ENGINE_MAP "${HEADER_PATH}/world/*.hpp")
file(GLOB HEADER_ENGINE_SCENE "${HEADER_PATH}/scene/*.hpp" "${HEADER_PATH}/scene/components/*.hpp")

# Source Files
set (SOURCES_PATH src)

file(GLOB SOURCES_ENGINE_CORE "${SOURCES_PATH}/core/*.cpp")
file(GLOB SOURCES_ENGINE_MAP "${SOURCES_PATH}/world/*.cpp")
file(GLOB SOURCES_ENGINE_SCENE "${SOURCES_PATH}/scene/*.cpp" "${SOURCES_PATH}/scene/components/*.cpp")

# Create Library
set (LIBRARY_NAME engine)
add_library(${LIBRARY_NAME} 
        STATIC 
                "${HEADER_PATH}/engine.hpp"

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
