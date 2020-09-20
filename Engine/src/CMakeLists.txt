# Source Files
set (SOURCES_ENGINE
        game.cpp
        window.cpp
        shader.cpp
        texture2D.cpp
        log.cpp)

# Header Files
set (HEADER_PATH ${PROJECT_SOURCE_DIR}/Engine/include)
set (HEADER_ENGINE 
        ${HEADER_PATH}/engine.h 
        ${HEADER_PATH}/game.h
        ${HEADER_PATH}/window.h
        ${HEADER_PATH}/shader.h
        ${HEADER_PATH}/texture2D.h
        ${HEADER_PATH}/stb_image.h
        ${HEADER_PATH}/log.h)

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
