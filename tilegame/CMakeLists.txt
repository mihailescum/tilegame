# Source Files
set (SOURCES_PATH src)
set (SOURCES_GAME
        ${SOURCES_PATH}/main.cpp 
        ${SOURCES_PATH}/tilegame.cpp
)

# set (SOURCES_WORLDSCENE_PATH worldscene)
# file (GLOB SOURCES_WORLDSCENE "${SOURCES_PATH}/${SOURCES_WORLDSCENE_PATH}/*.cpp")
        
# Header Files
set (HEADER_PATH include)
set (HEADER_GAME
        ${HEADER_PATH}/tilegame.hpp
)

# set (HEADER_WORLDSCENE_PATH worldscene)
# file (GLOB HEADER_WORLDSCENE "${HEADER_PATH}/${HEADER_WORLDSCENE_PATH}/*.hpp")
 
# Create Executable
set(EXECUTABLE_NAME tilegame)
add_executable(${EXECUTABLE_NAME} 
        ${SOURCES_GAME} 
        #${SOURCES_WORLDSCENE}
        
        ${HEADER_GAME}
        #${HEADER_WORLDSCENE}
)

set_target_properties(
        ${EXECUTABLE_NAME} 
        PROPERTIES 
        LINKER_LANGUAGE CXX)

# Include Project Header Files
target_include_directories(${EXECUTABLE_NAME} PRIVATE ${HEADER_PATH})
target_include_directories(${EXECUTABLE_NAME} PRIVATE ${PROJECT_SOURCE_DIR}/engine/include)
target_include_directories(${EXECUTABLE_NAME} PUBLIC ${PROJECT_SOURCE_DIR}/vendor/entt/include)

# Link with Engine
target_link_libraries(${EXECUTABLE_NAME} engine)