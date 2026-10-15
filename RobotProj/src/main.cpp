#include <iostream>
#include "RobotProtocol.hpp"
#include "CommandEncoder.hpp"
#include <string_view>
#include <CommandDecoder.hpp>

#ifdef __linux__
#include "SocketCanTransport.hpp"
#endif

void printPlatform() {
#if defined(_WIN32)
    std::cout << "Platform: Windows\n";
#elif defined(__linux__)
    std::cout << "Platform:Linux\n";

#if defined(__aarch64__)
    std::cout << "Architexture: ARM64\n";
#elif defined(__x84_64__)
    std::cout << "Architexture: x86_64\n";
#endif

#else
    std::cout << "Platform:Uknown\n";
#endif
}

void runDemo() {

    printPlatform();
    Command cmd{};
    cmd.node = NodeId::Head;
    cmd.command = CommandId::Set;
    cmd.data.resize(19);

    for (size_t i = 0; i < cmd.data.size(); ++i)
    {
        cmd.data[i] = static_cast<uint8_t>(i);
    }

    auto frames = encodeCommand(cmd, CanPriority::Normal);
    std::cout << "Total frames: " << frames.size() << '\n';

    for (const auto& frame : frames)
    {
        std::cout << frameToString(frame) << "\n\n";
    }
}
void printHelp()
{
    std::cout << "RobotProj CLI\n";
    std::cout << "Usage:\n";
    std::cout << "  --help       Show this help\n";
    std::cout << "  --demo       Run protocol demo\n";
    std::cout << "  --can IFACE  Listen on CAN interface\n";
}
#ifdef __linux__

int runCan(std::string_view interfaceName) {
    SocketCanTransport transport;
    CommandDecoder decoder;


    return 0;
}
#endif
int main(int argc, char* argv[])
{
    if (argc == 2 && std::string_view(argv[1]) == "--demo")
    {
        runDemo();
        return 0;
    }
    if (argc == 1 || (argc == 2 && std::string_view(argv[1]) == "--help"))
    {
        printHelp();
        return 0;
    }
    std::cerr << "Unknown arguments. Use --help\n";

    return 1;
}