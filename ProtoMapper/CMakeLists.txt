add_executable (ProtoMapper)

target_compile_options(ProtoMapper PRIVATE ${flags})

####	Meaning of the macros defined below.
#		
#		ROOT_DIR		- The location of the root folder, where fonts, images, shaders, data files, etc are lovated.
#		NOMINMAX		- To prevent windows.h from defining a 'ma' macro that interferes with entt.

target_compile_definitions(ProtoMapper PUBLIC ROOT_DIR="$ENV{ROOT_FOLDER}" NOMINMAX="1")

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
	${CPM_PACKAGE_GWork_SOURCE_DIR}/source/gwork/include
	${CPM_PACKAGE_GWork_SOURCE_DIR}/source/platform/include
)

target_link_libraries(
	ProtoMapper

	PRIVATE 

	glad::glad
	glm::glm
	glfw
	EnTT::EnTT	
	tinyxml2::tinyxml2
	Gwork
	sol2
)

target_sources(
	ProtoMapper

	PUBLIC 

	FILE_SET CXX_MODULES 

	BASE_DIRS

	${CMAKE_CURRENT_LIST_DIR}/src/

	FILES

	${CMAKE_CURRENT_LIST_DIR}/src/ProtoMapper.cpp
	${CMAKE_CURRENT_LIST_DIR}/src/Texture.cpp
	${CMAKE_CURRENT_LIST_DIR}/src/Shader.cpp
	${CMAKE_CURRENT_LIST_DIR}/src/Vertex.cpp
	${CMAKE_CURRENT_LIST_DIR}/src/Renderer.cpp
	${CMAKE_CURRENT_LIST_DIR}/src/Scene.cpp
	${CMAKE_CURRENT_LIST_DIR}/src/ResourceManager.cpp
	${CMAKE_CURRENT_LIST_DIR}/src/UIContainer.cpp
	${CMAKE_CURRENT_LIST_DIR}/src/UIRoot.cpp
	${CMAKE_CURRENT_LIST_DIR}/src/UILogFrame.cpp
	${CMAKE_CURRENT_LIST_DIR}/src/Image.cpp

	PRIVATE 

	${CMAKE_CURRENT_LIST_DIR}/src/main.cpp

)
