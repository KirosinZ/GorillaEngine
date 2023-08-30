set(HEADER_LIST
        "${GLFW_CPP_SOURCE_DIR}/include/glfw_cpp/glfw.hpp"
        "${GLFW_CPP_SOURCE_DIR}/include/glfw_cpp/library.hpp"
        "${GLFW_CPP_SOURCE_DIR}/include/glfw_cpp/monitor.hpp"
        "${GLFW_CPP_SOURCE_DIR}/include/glfw_cpp/vulkan_glue.hpp"
        "${GLFW_CPP_SOURCE_DIR}/include/glfw_cpp/window.hpp"
        #        "${GLFW_CPP_SOURCE_DIR}/include/glfw_cpp/windowhandle.hpp"
        "${GLFW_CPP_SOURCE_DIR}/include/glfw_cpp/windowhints.hpp")

add_library(glfw_cpp
        library.cpp
        monitor.cpp
        vulkan_glue.cpp
        window.cpp
        #        windowhandle.cpp
        ${HEADER_LIST})
add_library(Gorilla::glfw_cpp ALIAS glfw_cpp)

target_include_directories(glfw_cpp PUBLIC ../include)

target_link_libraries(glfw_cpp PUBLIC
        glfw
        glm
        Vulkan::Vulkan
        Gorilla::error_handling)

target_compile_features(glfw_cpp PUBLIC cxx_std_20)

source_group(
        TREE "${PROJECT_SOURCE_DIR}/include"
        PREFIX "Header Files"
        FILES ${HEADER_LIST})