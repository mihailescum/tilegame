# Source Files
set (SOURCES src/tinyxml2.cpp)
# Header Files
set (HEADER include/tinyxml2.h)

#Create Library
add_library(tinyxml2 ${SOURCES} ${HEADER})

# Include Project Header Files
target_include_directories(tinyxml2 PUBLIC include)