cmake_minimum_required(VERSION 3.25)

project(Cultivator)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_INTERPROCEDURAL_OPTIMIZATION_RELEASE ON)
set(CMAKE_VS_JUST_MY_CODE_DEBUGGING ON)

if (WIN32)
    add_compile_definitions(
            CLTV_SYSTEM_WINDOWS
            UNICODE
            _UNICODE
    )
else ()
    message(FATAL_ERROR "system not supported")
endif ()

if (MSVC)
    add_compile_options(/W4)
    add_link_options(/ENTRY:mainCRTStartup)
else ()
    add_compile_options(
            -Wall
            -Wextra
            -Wpedantic
            -Wshadow
            -Wconversion
            -Wsign-conversion
    )
endif ()

find_package(Vulkan)

file(GLOB_RECURSE sources CONFIGURE_DEPENDS src/*)

add_executable(Cultivator ${sources})
target_precompile_headers(Cultivator PRIVATE src/Precompiled.hpp)
target_include_directories(Cultivator PRIVATE
        src
        ${Vulkan_INCLUDE_DIRS}
)
