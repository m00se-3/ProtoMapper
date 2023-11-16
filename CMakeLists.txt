# CMakeList.txt : Top-level CMake project file, do global configuration
# and include sub-projects here.
#
cmake_minimum_required (VERSION 3.28)

project ("ProtoMapper" 
VERSION "$ENV{PROGRAM_VERSION}"
DESCRIPTION "$ENV{PROGRAM_DESCRIPTION}"
HOMEPAGE_URL "$ENV{PROGRAM_URL}"
LANGUAGES CXX C)

include("CPM.cmake")

# A few quick checks to ensure the compiler flags match the compiler being used.

if(${CMAKE_CXX_COMPILIER_ID} MATCHES MSVC)

set(ADDSanitize "")

if(${USE_ADDR_SANITIZER})
set(ADDSanitize "/fsanitize=address")
endif()

list(
	APPEND flags 

	"/W4"
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
	APPEND flags 

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

find_package(glm CONFIG REQUIRED)
find_package(glad CONFIG REQUIRED)
find_package(glfw3 CONFIG REQUIRED)
find_package(stb REQUIRED)
find_package(EnTT CONFIG REQUIRED)
find_package(sol2 CONFIG REQUIRED)
find_package(tinyxml2 CONFIG REQUIRED)
find_path(SIMPLEINI_INCLUDE_DIRS "ConvertUTF.c")

CPMAddPackage(
	NAME
	"Gwork"

	GIT_TAG
	"glad"

	GITHUB_REPOSITORY
	"m00se-3/GWork"

	OPTIONS 
	"RENDER_OPENGL_CORE"
	"USE_GLFW"
	"USE_GLAD"
	"USE_VCPKG"
)


# Include sub-projects.
add_subdirectory ("ProtoMapper")
