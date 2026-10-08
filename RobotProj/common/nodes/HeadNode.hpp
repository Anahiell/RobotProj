#pragma once

#include <cstdint>
#include "NodeId.hpp"

struct HeadNode
{
    static constexpr NodeId Id = NodeId::Head;

    enum class TargetBits : uint8_t
    {
        Yaw = 1 << 0,
        Pitch = 1 << 1
    };

    struct SetData
    {
        uint8_t mask{};

        int16_t yaw{};
        int16_t pitch{};

        void setYaw(int16_t value)
        {
            yaw = value;
            mask |= static_cast<uint8_t>(TargetBits::Yaw);
        }

        void setPitch(int16_t value)
        {
            pitch = value;
            mask |= static_cast<uint8_t>(TargetBits::Pitch);
        }
    };
};