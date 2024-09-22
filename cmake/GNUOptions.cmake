# Enable compiler flags for certain build configurations
if(${CMAKE_BUILD_TYPE} MATCHES Debug)
    list(
        APPEND ProtoCompilerFlags 
        "-g"
        "-D_GLIBCXX_ASSERTIONS"
    )
elseif(${CMAKE_BUILD_TYPE} MATCHES RelWithDebInfo)
    list(
        APPEND ProtoCompilerFlags 
        "-g"
        "-02"
        "-U_FORTIFY_SOURCE"
        "-D_FORTIFY_SOURCE=3"
        "-D_GLIBCXX_ASSERTIONS"
    )
else()
    list(
        APPEND ProtoCompilerFlags 
        "-02"
        "-U_FORTIFY_SOURCE"
        "-D_FORTIFY_SOURCE=2"
    )
endif()

# Enable special flags when using GCC
if(${CMAKE_CXX_COMPILIER_ID} MATCHES GNU)
    list(
        APPEND ProtoCompilerFlags 

        "-fhardened"
        "-Wbidi-chars=any"
        "-Wtrampolines"
        "-Whardened"
    )
endif()

#Exclude unsupported flags on Windows
if(NOT ${CMAKE_HOST_WIN32})
    list(
        APPEND ProtoCompilerFlags

        "-fPIC"
        "-fPIE"
        "-mbranch-protection=standard"
    )
endif()


# Enable flags for all builds
list(
	APPEND ProtoCompilerFlags 

	"-Wall" 
	"-Wextra" 
    "-Wformat"
    "-Wformat=2"
    "-Wimplicit-fallthrough"
    "-Werror=format-security"
    "-Werror=implicit"
    "-Werror=incompatible-pointer-types"
    "-Werror=int-conversion"
	"-Wpedantic" 
	"-Wno-newline-eof" 
	"-Wno-c++98-compat" 
	"-Wno-c++98-compat-pedantic"
	"-Wconversion"
    "-Wsign-conversion"

    "-fstrict-flex-arrays=3"
    "-fcf-protection=full"
    "-Wl,-z,nodlopen"
    "-Wl,-z,noexecstack"
    "-Wl,-z,relro"
    "-pie"
    "-shared"
    "-fno-strict-overflow"
    "-fno-strict-aliasing"
    "-ftrivial-auto-var-init=zero"
    "-fexceptions"
    "-Wl,--as-needed"
    "-Wl,--no-copy-dt-needed-entries"
)

# Enable sanitizers if chosen

if(${USE_ADDR_SANITIZER} MATCHES ON)
    list(
        APPEND ProtoCompilerFlags

        "-fsanitize=address"
    )
endif()

if(${USE_THREAD_SANITIZER} MATCHES ON)
    list(
        APPEND ProtoCompilerFlags

        "-fsanitize=thread"
    )
endif()

if(${USE_LEAK_SANITIZER} MATCHES ON)
    list(
        APPEND ProtoCompilerFlags

        "-fsanitize=leak"
    )
endif()

if(${USE_UND_SANITIZER} MATCHES ON)
    list(
        APPEND ProtoCompilerFlags

        "-fsanitize=undefined"
    )
endif()