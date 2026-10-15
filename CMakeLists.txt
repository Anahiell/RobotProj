
cmake_minimum_required(VERSION 3.20)

project(RobotProj LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Common robot logic
add_library(RobotCore STATIC
    RobotProj/src/HeadController.cpp
    RobotProj/common/protocol/RobotProtocol.cpp
    RobotProj/common/nodes/HeadNode.cpp
)

target_include_directories(RobotCore PUBLIC
    RobotProj/include
    RobotProj/common/protocol
    RobotProj/common/nodes
)

# Main application
add_executable(RobotProj
    RobotProj/src/main.cpp
)

target_link_libraries(RobotProj PRIVATE RobotCore)


# Testing
include(CTest)

if(BUILD_TESTING)
    include(FetchContent)

    set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)

    FetchContent_Declare(
        googletest
        URL https://github.com/google/googletest/archive/refs/tags/v1.17.0.zip
    )

    FetchContent_MakeAvailable(googletest)

    add_executable(RobotTests
        tests/RobotProtocolTests.cpp
    )

    target_link_libraries(RobotTests PRIVATE
        RobotCore
        GTest::gtest_main
    )

    include(GoogleTest)
    gtest_discover_tests(RobotTests)
endif()