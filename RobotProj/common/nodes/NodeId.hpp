#pragma once

#include <cstdint>

enum class NodeId : uint8_t
{
    Head = 1,
    Cooling,
    Arms,
    Base
};