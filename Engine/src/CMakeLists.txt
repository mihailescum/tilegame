# Source Files
set (SOURCES_ENGINE
        game.cpp
        window.cpp
        shader.cpp
        texture2D.cpp
        log.cpp
        graphicsdevice.cpp
        viewport.cpp
        spritebatch.cpp
        color.cpp)

# Header Files
set (HEADER_PATH ${PROJECT_SOURCE_DIR}/Engine/include)
set (HEADER_ENGINE 
        ${HEADER_PATH}/engine.hpp
        ${HEADER_PATH}/game.hpp
        ${HEADER_PATH}/window.hpp
        ${HEADER_PATH}/shader.hpp
        ${HEADER_PATH}/texture2D.hpp
        ${HEADER_PATH}/stb_image.hpp
        ${HEADER_PATH}/log.hpp
        ${HEADER_PATH}/graphicsdevice.hpp
        ${HEADER_PATH}/viewport.hpp
        ${HEADER_PATH}/rectangle.hpp
        ${HEADER_PATH}/spritebatch.hpp
        ${HEADER_PATH}/color.hpp)

# Create Library
set (LIBRARY_NAME engine)
add_library(${LIBRARY_NAME} STATIC ${SOURCES_ENGINE} ${HEADER_ENGINE})
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
