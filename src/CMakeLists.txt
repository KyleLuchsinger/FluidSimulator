cmake_minimum_required(VERSION 3.10)
project(fluid_simulation)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(fluid_sim
    main.cpp
    FluidSimulator.cpp
    Renderer.cpp
)

add_custom_command(
    TARGET fluid_sim POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E make_directory frames
    COMMENT "Creating frames directory"
)

if(MSVC)
    target_compile_options(fluid_sim PRIVATE
        /W4
        /MP
        /EHsc
        /permissive-
    )
    set_property(TARGET fluid_sim PROPERTY MSVC_RUNTIME_LIBRARY "MultiThreadedDLL")
else()
    target_compile_options(fluid_sim PRIVATE -Wall -Wextra -pedantic)
endif()

if(CMAKE_BUILD_TYPE STREQUAL "Release")
    if(MSVC)
        target_compile_options(fluid_sim PRIVATE /O2)
    else()
        target_compile_options(fluid_sim PRIVATE -O2)
    endif()
endif()