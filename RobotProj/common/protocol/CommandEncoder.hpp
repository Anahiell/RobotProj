#pragma once

#include "RobotProtocol.hpp"

#include<vector>

// Command <-> CAN Single frame
std::vector<CanFrame> encodeCommand(const Command& cmd, CanPriority priority = CanPriority::Normal);



