FetchContent_Declare(
        catch2
        GIT_REPOSITORY https://github.com/catchorg/Catch2.git
        GIT_TAG v3.4.0)
FetchContent_MakeAvailable(catch2)

add_executable(glfw_cpp_tests test.cpp)

target_compile_features(glfw_cpp_tests PRIVATE cxx_std_20)

target_link_libraries(glfw_cpp_tests glfw_cpp Catch2::Catch2WithMain)

add_test(NAME test1 COMMAND glfw_cpp_tests)