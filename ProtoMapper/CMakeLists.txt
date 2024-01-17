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
	${CMAKE_CURRENT_LIST_DIR}/include
	${CMAKE_CURRENT_LIST_DIR}/include/system
	${CMAKE_CURRENT_LIST_DIR}/include/ui
	${CMAKE_CURRENT_LIST_DIR}/include/renderer
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
	EnTT::EnTT	
	tinyxml2::tinyxml2
	nuklear
	sol2
	${LUA_LIBRARIES}
)

target_sources(
	ProtoMapper

	PUBLIC 

	# Headers

	${CMAKE_CURRENT_LIST_DIR}/include/ProtoMapper.hpp
	${CMAKE_CURRENT_LIST_DIR}/include/Scene.hpp
	
	${CMAKE_CURRENT_LIST_DIR}/include/ui/UIContainer.hpp
	${CMAKE_CURRENT_LIST_DIR}/include/ui/Font.hpp

	${CMAKE_CURRENT_LIST_DIR}/include/renderer/Image.hpp
	${CMAKE_CURRENT_LIST_DIR}/include/renderer/Renderer.hpp
	${CMAKE_CURRENT_LIST_DIR}/include/renderer/Shader.hpp
	${CMAKE_CURRENT_LIST_DIR}/include/renderer/Texture.hpp
	${CMAKE_CURRENT_LIST_DIR}/include/renderer/Vertex.hpp

	${CMAKE_CURRENT_LIST_DIR}/include/system/Logger.hpp
	${CMAKE_CURRENT_LIST_DIR}/include/system/LuaEngine.hpp
	${CMAKE_CURRENT_LIST_DIR}/include/system/RenderingSystem.hpp
	${CMAKE_CURRENT_LIST_DIR}/include/system/ResourceManager.hpp
	${CMAKE_CURRENT_LIST_DIR}/include/system/System.hpp
	${CMAKE_CURRENT_LIST_DIR}/include/system/Window.hpp

	PRIVATE 
	
	# Implementations
	
	${CMAKE_CURRENT_LIST_DIR}/src/ProtoMapper.cpp
	${CMAKE_CURRENT_LIST_DIR}/src/Scene.cpp
	
	${CMAKE_CURRENT_LIST_DIR}/src/ui/UIContainer.cpp
	${CMAKE_CURRENT_LIST_DIR}/src/ui/Font.cpp
	
	${CMAKE_CURRENT_LIST_DIR}/src/renderer/Texture.cpp
	${CMAKE_CURRENT_LIST_DIR}/src/renderer/Renderer.cpp
	${CMAKE_CURRENT_LIST_DIR}/src/renderer/Shader.cpp
	${CMAKE_CURRENT_LIST_DIR}/src/renderer/Vertex.cpp
	${CMAKE_CURRENT_LIST_DIR}/src/renderer/Image.cpp
	
	${CMAKE_CURRENT_LIST_DIR}/src/system/Logger.cpp
	${CMAKE_CURRENT_LIST_DIR}/src/system/LuaEngine.cpp
	${CMAKE_CURRENT_LIST_DIR}/src/system/RenderingSystem.cpp
	${CMAKE_CURRENT_LIST_DIR}/src/system/ResourceManager.cpp
	${CMAKE_CURRENT_LIST_DIR}/src/system/System.cpp
	${CMAKE_CURRENT_LIST_DIR}/src/system/Window.cpp

	${CMAKE_CURRENT_LIST_DIR}/src/main.cpp
	${CMAKE_CURRENT_LIST_DIR}/src/external/nuklear.cpp

)
