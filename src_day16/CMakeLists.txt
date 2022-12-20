## File: CMakeLists.txt
## Author: Bernhard Luedtke

project(advent LANGUAGES CXX)

find_package(OpenMP REQUIRED)
include_directories(${OpenMP_CXX_INCLUDE_DIRS})

set(SOURCES "main.cpp")

add_executable(advent ${SOURCES})

target_link_libraries(advent PRIVATE OpenMP::OpenMP_CXX)
