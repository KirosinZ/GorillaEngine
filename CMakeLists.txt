cmake_minimum_required(VERSION 3.25)
project(Gorilla)

#set(CMAKE_CXX_STANDARD 20)

find_package(Vulkan REQUIRED COMPONENTS
        shaderc_combined
        SPIRV-Tools)

add_subdirectory(external/boost-1.82.0 EXCLUDE_FROM_ALL)

set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)

#add_subdirectory(external/glfw-3.3.8)

add_compile_options(/std:c++latest)

add_compile_definitions(GORILLA_VERSION_MAJOR=0)
add_compile_definitions(GORILLA_VERSION_MINOR=0)
add_compile_definitions(GORILLA_VERSION_PATCH=1)

add_compile_definitions(GORILLA_BUILD_TYPE_${CMAKE_BUILD_TYPE})

add_subdirectory(external/glm)

#add_library(platform INTERFACE
#        src/platform/build.hpp
#        src/platform/common.hpp
#        src/platform/ver.hpp)
#
#add_library(error_handling STATIC
#        src/error_handling/assert.hpp
#        src/error_handling/assert.cpp
#        src/error_handling/exceptions.hpp)
#
#target_include_directories(error_handling
#        PUBLIC src)
#
#target_link_libraries(error_handling
#        platform
#        Boost::leaf)

#add_executable(Gorilla
#        src/platform/ver.hpp
#        src/platform/build.hpp
#        src/platform/common.hpp
#
#        external/imgui/imgui.cpp
#        external/imgui/imgui.h
#        external/imgui/imgui_impl_vulkan.cpp
#        external/imgui/imgui_impl_vulkan.h
#        external/imgui/imconfig.h
#        external/imgui/imgui_draw.cpp
#        external/imgui/imgui_demo.cpp
#        external/imgui/imgui_internal.h
#        external/imgui/imgui_tables.cpp
#        external/imgui/imgui_widgets.cpp
#        external/imgui/imgui_impl_glfw.cpp
#        external/imgui/imgui_impl_glfw.h
#
#        main.cpp src/glfw_cpp/library.hpp src/debugging/debugging.hpp src/glfw_cpp/windowhints.hpp src/debugging/log.hpp src/debugging/log.cpp src/glfw_cpp/window.hpp src/glfw_cpp/window.cpp src/glfw_cpp/monitor.cpp src/glfw_cpp/monitor.hpp src/glfw_cpp/glfw.hpp src/glfw_cpp/glfw.cpp src/glfw_cpp/library.cpp src/vulkan_renderer/environment.hpp src/glfw_cpp/vulkan_glue.hpp src/glfw_cpp/vulkan_glue.cpp src/vulkan_renderer/environment.cpp src/vulkan_renderer/display.cpp src/vulkan_renderer/display.hpp src/utility/reference_ptr.hpp)
#
#target_include_directories(Gorilla
#        PUBLIC src)
#
#target_link_libraries(Gorilla
#        Vulkan::Vulkan
#        Vulkan::shaderc_combined
#        Vulkan::SPIRV-Tools
#        error_handling
#        glfw
#        glm)

include(FetchContent)

add_subdirectory(libs/error_handling)
add_subdirectory(libs/glfw_cpp)

add_executable(Gorilla main.cpp)
target_link_libraries(Gorilla
        error_handling
        glfw_cpp
        Vulkan::Vulkan)