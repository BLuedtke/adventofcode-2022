## File: CMakeLists.txt
## Author: Bernhard Luedtke

cmake_minimum_required(VERSION 3.16)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

project(advent LANGUAGES CXX)

add_subdirectory(src_day5)

set_target_properties(advent PROPERTIES
                      RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin
)