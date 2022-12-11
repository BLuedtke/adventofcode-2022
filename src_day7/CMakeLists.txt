## File: CMakeLists.txt
## Author: Bernhard Luedtke

project(advent LANGUAGES CXX)

#find_package(Eigen3 REQUIRED)
#find_package(benchmark REQUIRED)

set(SOURCES "main.cpp")

#add_definitions     ( ${EIGEN3_DEFINITIONS} )
#include_directories ( ${EIGEN3_INCLUDE_DIRS} )

add_executable(advent ${SOURCES})

target_link_libraries(advent)
