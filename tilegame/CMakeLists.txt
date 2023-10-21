# Source Files
set (SOURCES_PATH src)
file (GLOB HEADERS_GAME "${SOURCES_PATH}/*.hpp")
file (GLOB SOURCES_GAME "${SOURCES_PATH}/*.cpp")

set (SOURCES_COMPONENTS_PATH components)
file (GLOB HEADERS_COMPONENTS "${SOURCES_PATH}/${SOURCES_COMPONENTS_PATH}/*.hpp")
file (GLOB SOURCES_COMPONENTS "${SOURCES_PATH}/${SOURCES_COMPONENTS_PATH}/*.cpp")

set (SOURCES_SYSTEMS_PATH systems)
file (GLOB HEADERS_SYSTEMS "${SOURCES_PATH}/${SOURCES_SYSTEMS_PATH}/*.hpp")
file (GLOB SOURCES_SYSTEMS "${SOURCES_PATH}/${SOURCES_SYSTEMS_PATH}/*.cpp")

set (SOURCES_WORLDSCENE_PATH worldscene)
file (GLOB HEADERS_WORLDSCENE "${SOURCES_PATH}/${SOURCES_WORLDSCENE_PATH}/*.hpp")
file (GLOB SOURCES_WORLDSCENE "${SOURCES_PATH}/${SOURCES_WORLDSCENE_PATH}/*.cpp")

set (
    HEADERS 
    ${HEADERS_GAME} 
    ${HEADERS_COMPONENTS} 
    ${HEADERS_SYSTEMS} 
    ${HEADERS_WORLDSCENE}
)
set (
    SOURCES 
    ${SOURCES_GAME} 
    ${SOURCES_COMPONENTS} 
    ${SOURCES_SYSTEMS} 
    ${SOURCES_WORLDSCENE}
)
 
# Create Executable
set(EXECUTABLE_NAME tilegame)
add_executable(${EXECUTABLE_NAME} ${SOURCES} ${HEADERS})

# Copy content files Files
add_custom_target(copy_assets
    COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_CURRENT_LIST_DIR}/content ${CMAKE_BINARY_DIR}/bin/content
)
add_custom_target(link_assets_debug
    COMMAND ${CMAKE_COMMAND} -E create_symlink ${CMAKE_BINARY_DIR}/bin/content ${CMAKE_BINARY_DIR}/content
)
add_dependencies(${EXECUTABLE_NAME} copy_assets)
add_dependencies(${EXECUTABLE_NAME} link_assets_debug)
add_dependencies(link_assets_debug copy_assets)

set_target_properties(${EXECUTABLE_NAME} PROPERTIES LINKER_LANGUAGE CXX)

# Include Project Header Files
target_include_directories(${EXECUTABLE_NAME} PRIVATE ${SOURCES_PATH})
target_include_directories(${EXECUTABLE_NAME} PRIVATE ${PROJECT_SOURCE_DIR}/engine/include)
target_include_directories(${EXECUTABLE_NAME} PRIVATE ${PROJECT_SOURCE_DIR}/vendor/entt/include)

# Link with Engine
target_link_libraries(${EXECUTABLE_NAME} engine)