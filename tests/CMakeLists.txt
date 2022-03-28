enable_testing()

Include(FetchContent)

FetchContent_Declare(
    Catch2
    GIT_REPOSITORY https://github.com/catchorg/Catch2.git
    GIT_TAG v3.0.0-preview3
)

FetchContent_MakeAvailable(Catch2)

add_executable(
    tests
    test.cpp
    ${CMAKE_SOURCE_DIR}/src/Firewall.cpp
    ${CMAKE_SOURCE_DIR}/src/Utils.cpp
    ${CMAKE_SOURCE_DIR}/src/LogWatcher.cpp
)

target_link_libraries(
    tests
    PUBLIC
    ole32.lib
    oleaut32.lib
    Qt6::Core
    Qt6::Sql
    Catch2::Catch2
)

target_include_directories(tests PRIVATE ${CMAKE_SOURCE_DIR}/src/)

list(APPEND CMAKE_MODULE_PATH ${catch2_SOURCE_DIR}/extras)
include(CTest)
include(Catch)
catch_discover_tests(tests)
