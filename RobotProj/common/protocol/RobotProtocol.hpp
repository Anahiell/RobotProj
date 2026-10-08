#include <cstdint>
#include <array>
#include <string>
#include <string_view>
#include <sstream>
#include "NodeId.hpp"
#include <HeadNode.hpp>
#include <CoolingNode.hpp>

enum class CommandId : uint8_t
{
	Set = 1,
	Stop,
	GetStatus,
	Status
};


struct Command
{
	NodeId node;
	CommandId command;
	uint8_t dataLength;
	std::array<uint8_t, 7> data;
};

struct CanFrame
{
	uint32_t id;
	uint8_t length;
	std::array<uint8_t, 8> data;
};


std::array<uint8_t, 2> encodeInt16(int16_t val);
int16_t decodeInt16(std::array<uint8_t, 2>pac);
//Command - CAN
CanFrame encodeCommand(const Command& cmd);
Command decodeCommand(const CanFrame& frame);

bool isValidNode(uint32_t id);
bool isValidCommand(uint8_t value);
bool isValidFrame(const CanFrame& frame);

std::string commandToString(const Command& cmd);
std::string_view toString(NodeId id);
std::string_view toString(CommandId id);
std::string frameToString(const CanFrame& frame);
