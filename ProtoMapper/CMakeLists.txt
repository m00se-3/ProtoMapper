add_executable (ProtoMapper)

target_compile_options(ProtoMapper PRIVATE ${ProtoCompilerFlags})

####	Meaning of the macros defined below.
#		
#		ROOT_DIR		- The location of the root folder, where fonts, images, shaders, data files, etc are lovated.
#		NOMINMAX		- To prevent windows.h from defining a 'ma' macro that interferes with entt.

target_compile_definitions(ProtoMapper PUBLIC ROOT_DIR="$ENV{ROOT_FOLDER}" NOMINMAX="1")

if(${CMAKE_BUILD_TYPE} MATCHES Debug)
	target_compile_definitions(ProtoMapper PUBLIC _DEBUG_)
elseif(${CMAKE_BUILD_TYPE} MATCHES Release)
	target_compile_definitions(ProtoMapper PUBLIC _RELEASE_)
elseif(${CMAKE_BUILD_TYPE} MATCHES RelWithDebInfo)
target_compile_definitions(ProtoMapper PUBLIC _RELWDEBUGSYM_)
endif()

target_include_directories(
	ProtoMapper

	PRIVATE 
	${CMAKE_CURRENT_LIST_DIR}/inc
	${SIMPLEINI_INCLUDE_DIRS}
	${stb_INCLUDE_DIR}
)

target_link_libraries(
	ProtoMapper

	PRIVATE 

	glad::glad
	glm::glm
	glfw
	EnTT::EnTT	
	tinyxml2::tinyxml2
	nuklear
	sol2
)

target_sources(
	ProtoMapper

	PUBLIC 

	FILE_SET CXX_MODULES 

	BASE_DIRS

	${CMAKE_CURRENT_LIST_DIR}/src/
	${CMAKE_CURRENT_LIST_DIR}/imports/

	FILES

	# Interfaces

	${CMAKE_CURRENT_LIST_DIR}/imports/ProtoMapper.ixx
	${CMAKE_CURRENT_LIST_DIR}/imports/Scene.ixx
	
	${CMAKE_CURRENT_LIST_DIR}/imports/ui/UIContainer.ixx

	${CMAKE_CURRENT_LIST_DIR}/imports/renderer/Image.ixx
	${CMAKE_CURRENT_LIST_DIR}/imports/renderer/Renderer.ixx
	${CMAKE_CURRENT_LIST_DIR}/imports/renderer/Shader.ixx
	${CMAKE_CURRENT_LIST_DIR}/imports/renderer/Texture.ixx
	${CMAKE_CURRENT_LIST_DIR}/imports/renderer/Vertex.ixx

	${CMAKE_CURRENT_LIST_DIR}/imports/system/Logger.ixx
	${CMAKE_CURRENT_LIST_DIR}/imports/system/LuaEngine.ixx
	${CMAKE_CURRENT_LIST_DIR}/imports/system/ResourceManager.ixx
	${CMAKE_CURRENT_LIST_DIR}/imports/system/Window.ixx
	
	# Implementations
	
	${CMAKE_CURRENT_LIST_DIR}/src/ProtoMapper.cpp
	${CMAKE_CURRENT_LIST_DIR}/src/Scene.cpp
	
	${CMAKE_CURRENT_LIST_DIR}/src/ui/UIContainer.cpp
	
	${CMAKE_CURRENT_LIST_DIR}/src/renderer/Texture.cpp
	${CMAKE_CURRENT_LIST_DIR}/src/renderer/Renderer.cpp
	${CMAKE_CURRENT_LIST_DIR}/src/renderer/Shader.cpp
	${CMAKE_CURRENT_LIST_DIR}/src/renderer/Vertex.cpp
	${CMAKE_CURRENT_LIST_DIR}/src/renderer/Image.cpp
	
	${CMAKE_CURRENT_LIST_DIR}/src/system/Logger.cpp
	${CMAKE_CURRENT_LIST_DIR}/src/system/LuaEngine.cpp
	${CMAKE_CURRENT_LIST_DIR}/src/system/ResourceManager.cpp
	${CMAKE_CURRENT_LIST_DIR}/src/system/Window.cpp

	PRIVATE 

	${CMAKE_CURRENT_LIST_DIR}/src/main.cpp
	${CMAKE_CURRENT_LIST_DIR}/src/external/nuklear.cpp

)
