﻿cmake_minimum_required (VERSION 3.25)

project ("ProtoMapper" 
VERSION "$ENV{PROGRAM_VERSION}"
DESCRIPTION "$ENV{PROGRAM_DESCRIPTION}"
HOMEPAGE_URL "$ENV{PROGRAM_URL}"
LANGUAGES CXX C)

include("CPM.cmake")
include(CTest)

include("cmake/Config.cmake")

if(ccache_FOUND)
    set(CMAKE_CXX_COMPILER_LAUNCHER ${ccache_FOUND})
    set(CPM_SOURCE_CACHE ${CMAKE_CURRENT_LIST_DIR}/.cache/CPM)
endif()

find_package(glm CONFIG REQUIRED)
find_package(glad CONFIG REQUIRED)
find_package(glfw3 CONFIG REQUIRED)
find_package(stb REQUIRED)
find_package(Lua REQUIRED)
find_package(tinyxml2 CONFIG REQUIRED)
find_package(unofficial-nuklear CONFIG REQUIRED)
find_path(SIMPLEINI_INCLUDE_DIRS "ConvertUTF.c")

cpmaddpackage("gh:m00se-3/sol2#fix-clang_18")
cpmaddpackage("gh:catchorg/Catch2@3.7.1")
cpmaddpackage("gh:gsl-lite/gsl-lite@0.41.0")

add_subdirectory ("ProtoMapper")
