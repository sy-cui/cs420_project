cmake_minimum_required(VERSION 3.10)

project(Main
    VERSION 1.0
    LANGUAGES CXX
)
set(CMAKE_CXX_COMPILER mpic++)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)

add_subdirectory(utils)

add_executable(Main main.cpp)

target_link_libraries(Main PUBLIC HelperLib)
target_include_directories(Main PUBLIC "${PROJECT_SOURCE_DIR}/utils")
