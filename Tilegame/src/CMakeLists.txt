# Source Files
set (SOURCES_GAME
        main.cpp 
        tilegame.cpp)
        
# Header Files
set (HEADER_PATH ${PROJECT_SOURCE_DIR}/Tilegame/include)
set (HEADER_GAME
        ${HEADER_PATH}/tilegame.hpp)
 
# Create Executable
set(EXECUTABLE_NAME tilegame)
add_executable(${EXECUTABLE_NAME} ${SOURCES_GAME} ${HEADER_GAME})
set_target_properties(${EXECUTABLE_NAME} PROPERTIES LINKER_LANGUAGE CXX)

# Copy contents
add_custom_command(TARGET tilegame POST_BUILD 
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${PROJECT_SOURCE_DIR}/Tilegame/content/ $<TARGET_FILE_DIR:tilegame>/content)

# Include Project Header Files
target_include_directories(${EXECUTABLE_NAME} PRIVATE ${HEADER_PATH})
target_include_directories(${EXECUTABLE_NAME} PRIVATE ${PROJECT_SOURCE_DIR}/Engine/include)

# Link with Engine
target_link_libraries(${EXECUTABLE_NAME} engine)

# Include glad Header Files
target_include_directories(${EXECUTABLE_NAME} PRIVATE ${PROJECT_SOURCE_DIR}/ExternalLibraries/glad/include)

# Link with Glad
#target_link_libraries(${EXECUTABLE_NAME} glad)

# Link with GLFW
#find_package(glfw3 3.3 REQUIRED)
#target_link_libraries(${EXECUTABLE_NAME} glfw)
