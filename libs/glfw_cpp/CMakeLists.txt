cmake_minimum_required(VERSION 3.15...3.25)

project(
        GLFW_CPP
        VERSION 0.1.0
        DESCRIPTION "GLFW wrapper for C++ with RAII handles"
        LANGUAGES CXX)

if (CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME)

    set(CMAKE_CXX_STANDARD 20)

    set(CMAKE_CXX_EXTENSIONS OFF)

    set_property(GLOBAL PROPERTY USE_FOLDERS ON)

    include(CTest)

    find_package(Doxygen)
    if (Doxygen_FOUND)
        add_subdirectory(docs)
    else ()
        message(STATUS "Doxygen not found, not building docs")
    endif ()
endif ()

add_subdirectory(src)

if (BUILD_TESTING)
    add_subdirectory(tests)
endif ()
