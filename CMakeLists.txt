cmake_minimum_required(VERSION 3.21.1)

project(TfIdf)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
# expected behaviour

include(FetchContent)
enable_testing()

include_directories(src)
add_subdirectory(src)