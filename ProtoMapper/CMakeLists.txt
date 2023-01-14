add_executable (ProtoMapper)

set_target_properties(ProtoMapper PROPERTIES CXX_STANDARD 20)
target_compile_options(ProtoMapper PRIVATE ${flags})
target_compile_definitions(ProtoMapper PUBLIC VERSION_NUMBER="$ENV{PROGRAM_VERSION}")

target_include_directories(
	ProtoMapper

	PUBLIC 
	${CMAKE_CURRENT_LIST_DIR}/inc
)

target_link_libraries(
	ProtoMapper

	PRIVATE 

	sfml-system
	sfml-network
	sfml-graphics
	sfml-window
	sfml-main
	tgui
)

target_sources(
	ProtoMapper

	PUBLIC 
	${CMAKE_CURRENT_LIST_DIR}/inc/ProtoMapper.hpp

	PRIVATE  
	${CMAKE_CURRENT_LIST_DIR}/src/ProtoMapper.cpp
)
