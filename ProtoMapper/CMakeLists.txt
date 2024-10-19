add_executable (ProtoMapper)

target_compile_options(ProtoMapper PRIVATE ${CompilerFlags})
target_link_options(ProtoMapper PRIVATE ${LinkerFlags})

####	Meaning of the macros defined below.
#		
#		ROOT_DIR		- The location of the root folder where fonts, images, shaders, data files, etc are relative to.
#		NOMINMAX		- To prevent windows.h from defining a 'ma' macro that interferes with entt.

target_compile_definitions(ProtoMapper PUBLIC ROOT_DIR="$ENV{ROOT_DIR}" NOMINMAX="1" SOL_ALL_SAFETIES_ON=1)

target_compile_definitions(
	ProtoMapper 
	PUBLIC 
	ASSETS_DIR="$ENV{ROOT_DIR}/assets"
	CONFIG_DIR="$ENV{ROOT_DIR}/config"
	UI_DIR="$ENV{ROOT_DIR}/config/ui"
	TEXT_DIR="$ENV{ROOT_DIR}/config/data/text"
	PROFILE_DIR="$ENV{ROOT_DIR}/config/profile"
	)

if(${CMAKE_BUILD_TYPE} MATCHES Debug)
	target_compile_definitions(ProtoMapper PUBLIC _DEBUG_)
elseif(${CMAKE_BUILD_TYPE} MATCHES Release)
	target_compile_definitions(ProtoMapper PUBLIC _RELEASE_)
elseif(${CMAKE_BUILD_TYPE} MATCHES RelWithDebInfo)
	target_compile_definitions(ProtoMapper PUBLIC _RELWDEBUGSYM_)
endif()

if(clang_tidy_FOUND)
  message("-- Enabling clang-tidy.\n")
  set_property(TARGET ProtoMapper PROPERTY CXX_CLANG_TIDY ${clang_tidy_FOUND})
endif()

target_include_directories(
	ProtoMapper

	PRIVATE 
	${CMAKE_CURRENT_LIST_DIR}/include
	${CMAKE_CURRENT_LIST_DIR}/include/stl
	${SIMPLEINI_INCLUDE_DIRS}
	${stb_INCLUDE_DIR}
	${LUA_INCLUDE_DIR}
)

target_link_libraries(
	ProtoMapper

	PRIVATE 

	glad::glad
	glm::glm
	glfw
	tinyxml2::tinyxml2
	nuklear
	gsl::gsl-lite
	gsl::gsl-lite-v0
	gsl::gsl-lite-v1
	sol2
	${LUA_LIBRARIES}
)

target_sources(
	ProtoMapper

	PUBLIC 

	# Headers

	${CMAKE_CURRENT_LIST_DIR}/include/ProtoMapper.hpp
	${CMAKE_CURRENT_LIST_DIR}/include/Scene.hpp
	${CMAKE_CURRENT_LIST_DIR}/include/UIContainer.hpp
	${CMAKE_CURRENT_LIST_DIR}/include/Font.hpp
	${CMAKE_CURRENT_LIST_DIR}/include/Image.hpp
	${CMAKE_CURRENT_LIST_DIR}/include/Renderer.hpp
	${CMAKE_CURRENT_LIST_DIR}/include/Shader.hpp
	${CMAKE_CURRENT_LIST_DIR}/include/Texture.hpp
	${CMAKE_CURRENT_LIST_DIR}/include/Vertex.hpp
	${CMAKE_CURRENT_LIST_DIR}/include/Window.hpp

	${CMAKE_CURRENT_LIST_DIR}/include/stl/resource.hpp
	${CMAKE_CURRENT_LIST_DIR}/include/stl/dyn_array.hpp

	PRIVATE 
	
	# Implementations
	
	${CMAKE_CURRENT_LIST_DIR}/src/ProtoMapper.cpp
	${CMAKE_CURRENT_LIST_DIR}/src/Scene.cpp
	${CMAKE_CURRENT_LIST_DIR}/src/UIContainer.cpp
	${CMAKE_CURRENT_LIST_DIR}/src/Font.cpp
	${CMAKE_CURRENT_LIST_DIR}/src/Texture.cpp
	${CMAKE_CURRENT_LIST_DIR}/src/Renderer.cpp
	${CMAKE_CURRENT_LIST_DIR}/src/Shader.cpp
	${CMAKE_CURRENT_LIST_DIR}/src/Vertex.cpp
	${CMAKE_CURRENT_LIST_DIR}/src/Image.cpp
	${CMAKE_CURRENT_LIST_DIR}/src/Window.cpp
	${CMAKE_CURRENT_LIST_DIR}/src/main.cpp

	${CMAKE_CURRENT_LIST_DIR}/src/external/nuklear.cpp

)
