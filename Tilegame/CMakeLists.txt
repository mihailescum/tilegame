# Source Files
set (SOURCES_PATH src)
set (SOURCES_GAME
        ${SOURCES_PATH}/main.cpp 
        ${SOURCES_PATH}/tilegame.cpp
        ${SOURCES_PATH}/worldscene.cpp
)
        
# Header Files
set (HEADER_PATH include)
set (HEADER_GAME
        ${HEADER_PATH}/tilegame.hpp
        ${HEADER_PATH}/worldscene.hpp
)
 
# Create Executable
set(EXECUTABLE_NAME tilegame)
add_executable(${EXECUTABLE_NAME} ${SOURCES_GAME} ${HEADER_GAME})
set_target_properties(${EXECUTABLE_NAME} PROPERTIES LINKER_LANGUAGE CXX)

# Copy contents
set (CONTENT_PATH ${CMAKE_CURRENT_SOURCE_DIR}/content)
add_custom_command(TARGET tilegame POST_BUILD 
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CONTENT_PATH} $<TARGET_FILE_DIR:tilegame>/content
        COMMENT "Copying content")

# Include Project Header Files
target_include_directories(${EXECUTABLE_NAME} PRIVATE ${HEADER_PATH})
target_include_directories(${EXECUTABLE_NAME} PRIVATE ${PROJECT_SOURCE_DIR}/Engine/include)
target_include_directories(${EXECUTABLE_NAME} PUBLIC ${PROJECT_SOURCE_DIR}/ExternalLibraries/entt/include)

# Link with Engine
target_link_libraries(${EXECUTABLE_NAME} engine)
