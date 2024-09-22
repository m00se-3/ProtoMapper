# Enable compiler flags for certain build configurations
if(NOT ${CMAKE_BUILD_TYPE} MATCHES Debug)
    list(
        APPEND ProtoCompilerFlags 
        "/O2"
    )
endif()

# Enable compiler flags for all builds
list(
	APPEND ProtoCompilerFlags 

	"/Wall"
	"/permissive-"
    "/sdl"
    "/DYNAMICBASE"
    "/HIGHENTROPYVA"
    "/guard:cf"
    "/guard:ehcont"
    "/QSpectre"
    "/QSpectre-load"
)

# Enable sanitizers if chosen

if(${USE_ADDR_SANITIZER} MATCHES ON)
    list(
        APPEND ProtoCompilerFlags

        "/fsanitize=address"
    )
endif()