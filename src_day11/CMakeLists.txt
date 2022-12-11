## File: CMakeLists.txt
## Author: Bernhard Luedtke

project(advent LANGUAGES CXX)

set(SOURCES "main.cpp")

add_executable(advent ${SOURCES})

target_link_libraries(advent)
