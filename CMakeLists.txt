# CMakeList.txt : Top-level CMake project file, do global configuration
# and include sub-projects here.
#
cmake_minimum_required (VERSION 3.25)

project ("ProtoMapper" 
VERSION "$ENV{PROGRAM_VERSION}"
DESCRIPTION "$ENV{PROGRAM_DESCRIPTION}"
HOMEPAGE_URL "$ENV{PROGRAM_URL}"
LANGUAGES CXX C)

find_program(CLANG_TIDY_LOC clang-tidy)

if(CLANG_TIDY_LOC)
  
  message("-- Enabling clang-tidy.\n")
  set(CMAKE_CXX_CLANG_TIDY ${CLANG_TIDY_LOC} -p ${CMAKE_BINARY_DIR})
endif()

# A few quick checks to ensure the compiler ProtoCompilerFlags match the compiler being used.

if(${CMAKE_CXX_COMPILIER_ID} MATCHES MSVC)

set(ADDSanitize "")

if(${USE_ADDR_SANITIZER})
set(ADDSanitize "/fsanitize=address")
endif()

list(
	APPEND ProtoCompilerFlags 

	"/Wall"
	"/permissive-"
	"/analyze"
	${ADDSanitize}
)

elseif((${CMAKE_CXX_COMPILIER_ID} MATCHES CLANG) OR (${CMAKE_CXX_COMPILIER_ID} MATCHES GNU))

set(ADDSanitize "")

if(${USE_ADDR_SANITIZER})
set(ADDSanitize "-fsanitize=address")
endif()

list(
	APPEND ProtoCompilerFlags 

	"-Wall" 
	"-Wextra" 
	"-Wpedantic" 
	"-Wno-newline-eof" 
	"-Wno-c++98-compat" 
	"-Wno-c++98-compat-pedantic"
	"-Wconversion"
	${ADDSanitize}
)

endif()

include("CPM.cmake")

find_package(glm CONFIG REQUIRED)
find_package(glad CONFIG REQUIRED)
find_package(glfw3 CONFIG REQUIRED)
find_package(stb REQUIRED)
find_package(EnTT CONFIG REQUIRED)
find_package(Lua REQUIRED)
find_package(tinyxml2 CONFIG REQUIRED)
find_package(unofficial-nuklear CONFIG REQUIRED)
find_path(SIMPLEINI_INCLUDE_DIRS "ConvertUTF.c")

CPMAddPackage("gh:m00se-3/sol2#fix-clang_18")

# Include sub-projects.
add_subdirectory ("ProtoMapper")
