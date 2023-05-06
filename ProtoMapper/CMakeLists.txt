add_executable (ProtoMapper)

set_target_properties(ProtoMapper PROPERTIES CXX_STANDARD 20)
target_compile_options(ProtoMapper PRIVATE ${flags})
target_compile_definitions(ProtoMapper PUBLIC VERSION_NUMBER="$ENV{PROGRAM_VERSION}" ASSETS_DIR="$ENV{ASSETS_FOLDER}")

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
)

target_link_libraries(
	ProtoMapper

	PRIVATE 

	glad::glad
	glm::glm
	SDL2::SDL2
	SDL2::SDL2main
	$<IF:$<TARGET_EXISTS:SDL2_ttf::SDL2_ttf>,SDL2_ttf::SDL2_ttf,SDL2_ttf::SDL2_ttf-static>
	$<IF:$<TARGET_EXISTS:SDL2_image::SDL2_image>,SDL2_image::SDL2_image,SDL2_image::SDL2_image-static>
	tgui
)

target_sources(
	ProtoMapper

	PUBLIC 
	${CMAKE_CURRENT_LIST_DIR}/inc/ProtoMapper.hpp
	${CMAKE_CURRENT_LIST_DIR}/inc/Map.hpp
	${CMAKE_CURRENT_LIST_DIR}/inc/HeightMap2D.hpp
	${CMAKE_CURRENT_LIST_DIR}/inc/TopMenu.hpp
	${CMAKE_CURRENT_LIST_DIR}/inc/UIGroup.hpp
	${CMAKE_CURRENT_LIST_DIR}/inc/Texture.hpp
	${CMAKE_CURRENT_LIST_DIR}/inc/Shader.hpp
	${CMAKE_CURRENT_LIST_DIR}/inc/Vertex.hpp
	${CMAKE_CURRENT_LIST_DIR}/inc/Renderer.hpp

	PRIVATE  
	${CMAKE_CURRENT_LIST_DIR}/src/ProtoMapper.cpp
	${CMAKE_CURRENT_LIST_DIR}/src/Map.cpp
	${CMAKE_CURRENT_LIST_DIR}/src/HeightMap2D.cpp
	${CMAKE_CURRENT_LIST_DIR}/src/TopMenu.cpp
	${CMAKE_CURRENT_LIST_DIR}/src/UIGroup.cpp
	${CMAKE_CURRENT_LIST_DIR}/src/Texture.cpp
	${CMAKE_CURRENT_LIST_DIR}/src/Shader.cpp
	${CMAKE_CURRENT_LIST_DIR}/src/Vertex.cpp
	${CMAKE_CURRENT_LIST_DIR}/src/Renderer.cpp
)
