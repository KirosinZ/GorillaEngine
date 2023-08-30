FetchContent_Declare(
        catch2
        GIT_REPOSITORY https://github.com/catchorg/Catch2.git
        GIT_TAG v3.4.0)
FetchContent_MakeAvailable(catch2)

add_executable(error_handling_tests test2.cpp)

target_compile_features(error_handling_tests PRIVATE cxx_std_20)

target_link_libraries(error_handling_tests lib Catch2::Catch2WithMain)

add_test(NAME test1 COMMAND error_handling_tests)