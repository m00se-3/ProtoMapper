cmake_minimum_required (VERSION 3.25)

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
    file(WRITE ${CPM_SOURCE_CACHE}/.clang-tidy "Checks: '-*'")
endif()

find_package(glm CONFIG REQUIRED)
find_package(glad CONFIG REQUIRED)
find_package(glfw3 CONFIG REQUIRED)
find_package(stb REQUIRED)

cpmaddpackage(
  NAME lua
  GITHUB_REPOSITORY lua/lua
  VERSION 5.4.6
  DOWNLOAD_ONLY Yes
)

if(lua_ADDED)

  FILE(GLOB lua_sources ${lua_SOURCE_DIR}/*.c)
  list(REMOVE_ITEM lua_sources ${lua_SOURCE_DIR}/lua.c ${lua_SOURCE_DIR}/luac.c)

  add_library(lua STATIC ${lua_sources})

  target_include_directories(
    lua
    PRIVATE
    ${lua_SOURCE_DIR}
  )

endif()

cpmaddpackage("gh:leethomason/tinyxml2@10.0.0")
cpmaddpackage("gh:brofield/simpleini@4.22")
cpmaddpackage("gh:Immediate-Mode-UI/Nuklear#4.12.2")
cpmaddpackage("gh:m00se-3/sol2#fix-clang_18")
cpmaddpackage("gh:catchorg/Catch2@3.7.1")
cpmaddpackage("gh:gsl-lite/gsl-lite@0.41.0")

add_subdirectory ("ProtoMapper")
