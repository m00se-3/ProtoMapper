set(ProtoMapper_Root_Dir ${CMAKE_CURRENT_LIST_DIR}/../ProtoMapper)

# Add commands for the tests
add_executable(shared_res_test_exe ${ProtoMapper_Root_Dir}/tests/shared_res_test.cpp)

if(clang_tidy_FOUND)
	set_property(TARGET shared_res_test_exe PROPERTY CXX_CLANG_TIDY ${clang_tidy_FOUND})
endif()

# Build options and libraries

target_include_directories(shared_res_test_exe PUBLIC ${ProtoMapper_Root_Dir}/include/stl ${catch2_SOURCE_DIR})
target_compile_options(shared_res_test_exe PRIVATE ${CompilerFlags})
target_link_options(shared_res_test_exe PRIVATE ${LinkerFlags})
target_compile_definitions(shared_res_test_exe PUBLIC TEST_DIR="$ENV{ROOT_DIR}/ProtoMapper/tests")
target_link_libraries(
	shared_res_test_exe

	PUBLIC 

	Catch2::Catch2WithMain
	gsl::gsl-lite
	gsl::gsl-lite-v0
	gsl::gsl-lite-v1
)

# Create tests

add_test(NAME shared_res_test COMMAND shared_res_test_exe)