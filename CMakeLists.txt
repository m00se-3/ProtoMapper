# CMakeList.txt : Top-level CMake project file, do global configuration
# and include sub-projects here.
#
cmake_minimum_required (VERSION 3.21)

project ("ProtoMapper" 
VERSION "$ENV{PROGRAM_VERSION}"
DESCRIPTION "$ENV{PROGRAM_DESCRIPTION}"
HOMEPAGE_URL "$ENV{PROGRAM_URL}"
LANGUAGES CXX)

list(APPEND flags "-Wall" "-Wextra" "-Wpedantic" "-Wno-newline-eof" "-Wno-c++98-compat" "-Wno-c++98-compat-pedantic")

find_package(glm CONFIG REQUIRED)
find_package(glad CONFIG REQUIRED)
find_package(glfw3 CONFIG REQUIRED)
find_package(stb REQUIRED)
find_package(unofficial-nuklear CONFIG REQUIRED)
find_package(EnTT CONFIG REQUIRED)
find_path(SIMPLEINI_INCLUDE_DIRS "ConvertUTF.c")
find_path(CHAISCRIPT_INCLUDE_DIRS "chaiscript/chaiscript.hpp")

# Include sub-projects.
add_subdirectory ("ProtoMapper")
