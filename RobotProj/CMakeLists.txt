# CMakeList.txt : CMake project for RobotProj, include source and define
# project specific logic here.
#

# Add source to this project's executable.
add_executable (RobotProj   "src/main.cpp" "src/HeadController.cpp")

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET RobotProj PROPERTY CXX_STANDARD 20)
endif()

# TODO: Add tests and install targets if needed.
