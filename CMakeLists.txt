## File: CMakeLists.txt
## Author: Bernhard Luedtke

cmake_minimum_required(VERSION 3.16)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

project(advent LANGUAGES CXX)

include(CheckCXXCompilerFlag)
CHECK_CXX_COMPILER_FLAG("-march=native" COMPILER_SUPPORTS_MARCH_NATIVE)
if(COMPILER_SUPPORTS_MARCH_NATIVE)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -march=native")
	message("Set march-native")
endif()

add_subdirectory(src_day16)

set_target_properties(advent PROPERTIES
                      RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin
)