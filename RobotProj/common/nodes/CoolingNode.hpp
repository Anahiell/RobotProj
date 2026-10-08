#pragma once

#include <cstdint>
#include "NodeId.hpp"

struct CoolingNode
{
    static constexpr NodeId Id = NodeId::Cooling;

    enum class Target : uint8_t
    {
        FanIn = 1 << 0,
        FanEx = 1 << 1
    };
};