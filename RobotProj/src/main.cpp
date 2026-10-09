#include <iostream>

#include "RobotProtocol.hpp"
#include "HeadNode.hpp"

int main()
{
    std::cout << "Robot project started\n";

    HeadNode head;

    head.setYaw(1700);
    head.setPitch(1450);

    Command original = head.makeCommand();

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