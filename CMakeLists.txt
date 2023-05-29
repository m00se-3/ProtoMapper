# CMakeList.txt : Top-level CMake project file, do global configuration
# and include sub-projects here.
#
cmake_minimum_required (VERSION 3.21)

project ("ProtoMapper" LANGUAGES CXX)

list(APPEND flags "-Wall" "-Wextra" "-Wpedantic" "-Wno-newline-eof" "-Wno-c++98-compat" "-Wno-c++98-compat-pedantic")

find_package(glm CONFIG REQUIRED)
find_package(glad CONFIG REQUIRED)
find_package(SDL2 CONFIG REQUIRED)
find_package(sdl2_image CONFIG REQUIRED)
find_package(unofficial-nuklear CONFIG REQUIRED)
find_package(EnTT CONFIG REQUIRED)
find_path(SIMPLEINI_INCLUDE_DIRS "ConvertUTF.c")

# Include sub-projects.
add_subdirectory ("ProtoMapper")
