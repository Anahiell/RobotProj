#include <iostream>

#include "RobotProtocol.hpp"

int main()
{
    std::cout << "Robot project started\n";

    Command original =
        makeSetCommand(HeadNode::Target::Yaw, -1700);

    CanFrame frame = encodeCommand(original);

    if (!isValidFrame(frame))
    {
        std::cout << "Invalid frame\n";
        return 1;
    }

    Command decoded = decodeCommand(frame);

    std::cout
        << "\nORIGINAL\n"
        << commandToString(original)

        << "\n\nFRAME\n"
        << frameToString(frame)

        << "\n\nDECODED\n"
        << commandToString(decoded)
        << std::endl;

    return 0;
}