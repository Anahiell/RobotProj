#pragma once

#include <array>
#include <cstdint>
#include <string>
#include <string_view>
#include <vector>


enum class NodeId : uint8_t
{
    Broadcast = 0,
    Head = 1,
    Cooling = 2,
    Arms = 3,
    Base = 4
};


// Transport type.
// Single 
// Sequence 
enum class FrameType : uint8_t
{
    Single = 0,
    SequenceStart,
    SequenceData,
    SequenceEnd,
    Ack
};

enum class CanPriority : uint8_t
{
    Critical = 0,
    High = 1,
    Normal = 3,
    Low = 7
};


enum class CommandId : uint8_t
{
    Set = 1,
    Stop,
    GetStatus,
    Status,
    Heartbeat
};


struct Command
{
    NodeId node{};
    CommandId command{};

    std::vector<uint8_t> data{};
};


struct CanIdFields
{
    CanPriority priority{};
    NodeId node{};
    FrameType type{};
};


struct CanFrame
{
    // Standard CAN ID
    uint32_t id{};

    uint8_t length{};
    std::array<uint8_t, 8> data{};
};


std::array<uint8_t, 2> encodeInt16(int16_t value);
int16_t decodeInt16(std::array<uint8_t, 2> data);

std::array<uint8_t, 4> encodeInt32(int32_t value);
int32_t decodeInt32(std::array<uint8_t, 4> data);


// 11-bit CAN ID
//
// bits [10:8] = priority
// bits [7:3]  = node
// bits [2:0]  = frame type

uint32_t encodeCanId(
    CanPriority priority,
    NodeId node,
    FrameType type
);

CanIdFields decodeCanId(uint32_t id);


// Command <-> CAN Single frame

std::vector<CanFrame> encodeCommand(
    const Command& cmd,
    CanPriority priority = CanPriority::Normal
);

Command decodeCommand(const CanFrame& frame);


bool isValidNode(NodeId node);
bool isValidCommand(CommandId command);
bool isValidFrameType(FrameType type);

bool isValidFrame(const CanFrame& frame);


std::string commandToString(const Command& cmd);
std::string frameToString(const CanFrame& frame);

std::string_view toString(NodeId id);
std::string_view toString(CommandId id);
std::string_view toString(FrameType type);