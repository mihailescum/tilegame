# Source Files
set (SOURCES pugixml.cpp)
# Header Files
set (HEADER pugixml.hpp pugiconfig.hpp)

#Create Library
add_library(pugixml ${SOURCES} ${HEADER})

# Include Project Header Files
target_include_directories(pugixml PUBLIC include)