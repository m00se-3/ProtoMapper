set(ProtoMapper_Root_Dir ${CMAKE_CURRENT_LIST_DIR}/../ProtoMapper)

# Add commands for the tests
add_executable(dyn_array_test_exe ${ProtoMapper_Root_Dir}/tests/dyn_array_test.cpp)

if(clang_tidy_FOUND)
	#set_property(TARGET dyn_array_test_exe PROPERTY CXX_CLANG_TIDY ${clang_tidy_FOUND})
endif()

# Build options and libraries

target_include_directories(dyn_array_test_exe PUBLIC ${ProtoMapper_Root_Dir}/include/stl ${catch2_SOURCE_DIR})
target_compile_options(dyn_array_test_exe PRIVATE ${CompilerFlags})
target_link_options(dyn_array_test_exe PRIVATE ${LinkerFlags})
target_link_libraries(
	dyn_array_test_exe

	PUBLIC 

	Catch2::Catch2WithMain
	gsl::gsl-lite
	gsl::gsl-lite-v0
	gsl::gsl-lite-v1
)

# Create tests

add_test(NAME dyn_array_test COMMAND dyn_array_test_exe)