#CMake Version
cmake_minimum_required (VERSION 3.10)

#Project Configuration
project (BlockWorld) 
set (CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

#Subdirectories
add_subdirectory(ExternalLibraries/glad)
add_subdirectory(ExternalLibraries/tinyxml2)
add_subdirectory(Engine)
add_subdirectory(Tilegame)
