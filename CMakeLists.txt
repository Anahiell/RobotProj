cmake_minimum_required(VERSION 3.20)

project(RobotProj LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(RobotProj
    RobotProj/src/main.cpp
    RobotProj/src/HeadController.cpp
    RobotProj/common/protocol/RobotProtocol.cpp
 "RobotProj/common/nodes/HeadNode.cpp")

target_include_directories(RobotProj PRIVATE
    RobotProj/include
    RobotProj/common/protocol
    RobotProj/common/nodes

)