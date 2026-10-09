#pragma once

#include <cstdint>
#include "../protocol/RobotProtocol.hpp"

class HeadNode
{
private:
    int16_t yaw_{};
    int16_t pitch_{};

    uint8_t changedMask_{};

public:
    static constexpr NodeId Id = NodeId::Head;

    enum TargetBits : uint8_t
    {
        Yaw = 1 << 0,
        Pitch = 1 << 1
    };

    void setYaw(int16_t value);
    void setPitch(int16_t value);

    Command makeCommand();
};