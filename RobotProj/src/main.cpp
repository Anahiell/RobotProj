#include <iostream>
#include "RobotProtocol.hpp"

void printPlatform() {
#if defined(_WIN32)
    std::cout << "Platform: Windows\n";
#elif defined(_linux_)
    std::cout << "Platform:Linux\n";

#if defined(_aarch64_)
    std::cout << "Architexture: ARM64\n";
#elif defined(_x84_64_)
    std::cout << "Architexture: x86_64\n";
#endif

#else
    std::cout << "Platform:Uknown\n";
#endif
}

int main()
{
    printPlatform();
    Command cmd{};

    cmd.node = NodeId::Head;
    cmd.command = CommandId::Set;
    cmd.data.resize(19);

    for (size_t i = 0; i < cmd.data.size(); ++i)
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