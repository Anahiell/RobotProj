#include <iostream>
#include "RobotProtocol.hpp"

int main()
{
    Command cmd{};

    cmd.node = NodeId::Head;
    cmd.command = CommandId::Set;
    cmd.dataLength = 19;

    for (size_t i = 0; i < cmd.dataLength; ++i)
    {
        cmd.data[i] = static_cast<uint8_t>(i);
    }

    auto frames = encodeCommand(cmd, CanPriority{});

    std::cout << "Total frames: " << frames.size() << '\n';

    for (const auto& frame : frames)
    {
        std::cout << frameToString(frame) << "\n\n";
    }
}