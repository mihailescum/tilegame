# Source Files
set (SOURCES_PATH src)
set (SOURCES_GAME
        ${SOURCES_PATH}/main.cpp 
        ${SOURCES_PATH}/tilegame.cpp
)

set (SOURCES_WORLDSCENE_PATH worldscene)
set (SOURCES_WORLDSCENE
        ${SOURCES_PATH}/${SOURCES_WORLDSCENE_PATH}/worldscene.cpp
        ${SOURCES_PATH}/${SOURCES_WORLDSCENE_PATH}/contentsystem.cpp
        ${SOURCES_PATH}/${SOURCES_WORLDSCENE_PATH}/inputsystem.cpp
        ${SOURCES_PATH}/${SOURCES_WORLDSCENE_PATH}/camerasystem.cpp
        ${SOURCES_PATH}/${SOURCES_WORLDSCENE_PATH}/movementsystem.cpp
)
        
# Header Files
set (HEADER_PATH include)
set (HEADER_GAME
        ${HEADER_PATH}/tilegame.hpp
)

set (HEADER_WORLDSCENE_PATH worldscene)
set (HEADER_WORLDSCENE
        ${HEADER_PATH}/${HEADER_WORLDSCENE_PATH}/worldscene.hpp
        ${HEADER_PATH}/${HEADER_WORLDSCENE_PATH}/contentsystem.hpp
        ${HEADER_PATH}/${HEADER_WORLDSCENE_PATH}/inputsystem.hpp
        ${HEADER_PATH}/${HEADER_WORLDSCENE_PATH}/camerasystem.hpp
        ${HEADER_PATH}/${HEADER_WORLDSCENE_PATH}/movementsystem.hpp
)
 
# Create Executable
set(EXECUTABLE_NAME tilegame)
add_executable(${EXECUTABLE_NAME} 
        ${SOURCES_GAME} 
        ${SOURCES_WORLDSCENE}
        
        ${HEADER_GAME}
        ${HEADER_WORLDSCENE}
)
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
