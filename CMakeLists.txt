# CMakeList.txt : Top-level CMake project file, do global configuration
# and include sub-projects here.
#
cmake_minimum_required (VERSION 3.21)

project ("ProtoMapper" LANGUAGES CXX)

list(APPEND flags "-Wall" "-Wextra" "-Wpedantic" "-Wno-newline-eof" "-Wno-c++98-compat" "-Wno-c++98-compat-pedantic")

find_package(SFML COMPONENTS system window graphics CONFIG REQUIRED)
find_package(fmt CONFIG REQUIRED)
find_package(TGUI CONFIG REQUIRED)

# Include sub-projects.
add_subdirectory ("ProtoMapper")
