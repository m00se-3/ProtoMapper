add_executable (ProtoMapper)

set_target_properties(ProtoMapper PROPERTIES CXX_STANDARD 20)
target_compile_options(ProtoMapper PRIVATE ${flags})

####	Meaning of the macros defined below.
#		
#		VERSION_NUMBER  - The version number of the program. (Never would've guessed that, huh?')
#		ASSETS_DIR		- The location of the root folder, where fonts, images, shaders, data files, etc are lovated.
#		NOMINMAX		- To prevent windows.h from defining a 'ma' macro that interferes with entt.

target_compile_definitions(ProtoMapper PUBLIC VERSION_NUMBER="$ENV{PROGRAM_VERSION}" ROOT_DIR="$ENV{ROOT_FOLDER}" NOMINMAX="1")

if(${CMAKE_BUILD_TYPE} MATCHES Debug)
	target_compile_definitions(ProtoMapper PUBLIC _DEBUG_)
elseif(${CMAKE_BUILD_TYPE} MATCHES Release AND NOT $ENV{POST_BUILD_TYPE} MATCHES Distro)
	target_compile_definitions(ProtoMapper PUBLIC _RELEASE_)
endif()

target_include_directories(
	ProtoMapper

	PRIVATE 
	${CMAKE_CURRENT_LIST_DIR}/inc
	${SIMPLEINI_INCLUDE_DIRS}
	${stb_INCLUDE_DIR}
	${CHAISCRIPT_INCLUDE_DIRS}
)

target_link_libraries(
	ProtoMapper

	PRIVATE 

	glad::glad
	glm::glm
	glfw
	nuklear
	EnTT::EnTT
	nlohmann_json::nlohmann_json
)

target_sources(
	ProtoMapper

	PUBLIC 
	${CMAKE_CURRENT_LIST_DIR}/inc/ProtoMapper.hpp
	${CMAKE_CURRENT_LIST_DIR}/inc/Texture.hpp
	${CMAKE_CURRENT_LIST_DIR}/inc/Image.hpp
	${CMAKE_CURRENT_LIST_DIR}/inc/Shader.hpp
	${CMAKE_CURRENT_LIST_DIR}/inc/Vertex.hpp
	${CMAKE_CURRENT_LIST_DIR}/inc/Renderer.hpp
	${CMAKE_CURRENT_LIST_DIR}/inc/Scene.hpp
	${CMAKE_CURRENT_LIST_DIR}/inc/ResourceManager.hpp


	${CMAKE_CURRENT_LIST_DIR}/inc/components/All.hpp
	${CMAKE_CURRENT_LIST_DIR}/inc/components/Common.hpp
	${CMAKE_CURRENT_LIST_DIR}/inc/components/UI.hpp

	PRIVATE  
	${CMAKE_CURRENT_LIST_DIR}/src/external/nuklear.cpp
	${CMAKE_CURRENT_LIST_DIR}/src/ProtoMapper.cpp
	${CMAKE_CURRENT_LIST_DIR}/src/Texture.cpp
	${CMAKE_CURRENT_LIST_DIR}/src/Image.cpp
	${CMAKE_CURRENT_LIST_DIR}/src/Shader.cpp
	${CMAKE_CURRENT_LIST_DIR}/src/Vertex.cpp
	${CMAKE_CURRENT_LIST_DIR}/src/Renderer.cpp
	${CMAKE_CURRENT_LIST_DIR}/src/Scene.cpp
	${CMAKE_CURRENT_LIST_DIR}/src/ResourceManager.cpp


	${CMAKE_CURRENT_LIST_DIR}/src/components/Common.cpp
	${CMAKE_CURRENT_LIST_DIR}/src/components/UI.cpp
)
