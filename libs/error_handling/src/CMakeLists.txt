set(HEADER_LIST
        "${ERROR_HANDLING_SOURCE_DIR}/include/error_handling/error_handling.hpp"
        "${ERROR_HANDLING_SOURCE_DIR}/include/error_handling/assert.hpp"
        "${ERROR_HANDLING_SOURCE_DIR}/include/error_handling/exceptions.hpp")

add_library(error_handling
        assert.cpp
        ${HEADER_LIST})
add_library(Gorilla::error_handling ALIAS error_handling)

target_include_directories(error_handling PUBLIC
        ../include
        ${Boost_INCLUDE_DIR})

target_compile_features(error_handling PUBLIC cxx_std_20)

source_group(
        TREE "${PROJECT_SOURCE_DIR}/include"
        PREFIX "Header Files"
        FILES ${HEADER_LIST})