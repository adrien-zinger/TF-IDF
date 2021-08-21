cmake_minimum_required(VERSION 3.19)

project(Basic)

include(FetchContent)
enable_testing()
FetchContent_Declare(
  googletest
  GIT_REPOSITORY https://github.com/google/googletest.git
  GIT_TAG        release-1.11.0
)
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

include_directories(src)
add_subdirectory(src)
add_subdirectory(tests)