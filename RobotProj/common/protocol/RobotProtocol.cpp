#include "RobotProtocol.hpp"
#include <iomanip>

Command makeCommand(HeadNode::Target target, int16_t value)
{
	Command cmd{};

	cmd.node = HeadNode::Id;
	cmd.command = CommandId::Set;

	auto valueBytes = encodeInt16(value);
	uint8_t mask =
		static_cast<uint8_t>(HeadNode::TargetBits::Yaw) |
		static_cast<uint8_t>(HeadNode::TargetBits::Pitch);
	cmd.data[0] = mask;
	cmd.data[1] = valueBytes[0];
	cmd.data[2] = valueBytes[1];

	cmd.dataLength = 3;

	return cmd;
}
Command makeSetCommand(CoolingNode::Target target, int16_t value)
{
	Command cmd{};

	cmd.node = CoolingNode::Id;
	cmd.command = CommandId::Set;

	auto valueBytes = encodeInt16(value);

	cmd.data[0] = static_cast<uint8_t>(target);
	cmd.data[1] = valueBytes[0];
	cmd.data[2] = valueBytes[1];

	cmd.dataLength = 3;

	return cmd;
}
std::array<uint8_t, 2> encodeInt16(int16_t val)
{

	uint16_t raw = static_cast<uint16_t>(val);
	std::array<uint8_t, 2> res;
	res[0] = static_cast<uint8_t> (raw >> 8); //HIGHT
	res[1] = static_cast<uint8_t> (raw & 0xFF); //LOW

	return res;
}

int16_t decodeInt16(std::array<uint8_t, 2> pac)
{
	uint16_t raw = (static_cast<uint16_t>(pac[0]) << 8) | static_cast<uint16_t>(pac[1]);
	return static_cast<int16_t>(raw);
}

CanFrame encodeCommand(const Command& cmd)
{
	CanFrame frame{};

	frame.id = static_cast<uint32_t>(cmd.node);

	frame.data[0] = static_cast<uint8_t>(cmd.command);

	for (uint8_t i = 0; i < cmd.dataLength; ++i)
	{
		frame.data[i + 1] = cmd.data[i];
	}

	frame.length = cmd.dataLength + 1;

	return frame;
}

Command decodeCommand(const CanFrame& frame)
{
	Command cmd{};

	cmd.node = static_cast<NodeId>(frame.id);
	cmd.command = static_cast<CommandId>(frame.data[0]);

	cmd.dataLength = frame.length - 1;

	for (uint8_t i = 0; i < cmd.dataLength; ++i)
	{
		cmd.data[i] = frame.data[i + 1];
	}

	return cmd;
}

bool isValidNode(uint32_t id)
{
	NodeId node = static_cast<NodeId>(id);

	switch (node)
	{
	case NodeId::Head:
	case NodeId::Cooling:
	case NodeId::Arms:
	case NodeId::Base:
		return true;

	default:
		return false;
	}
}

bool isValidCommand(uint8_t value)
{
	CommandId command = static_cast<CommandId>(value);

	switch (command)
	{
	case CommandId::Set:
	case CommandId::Stop:
	case CommandId::GetStatus:
	case CommandId::Status:
		return true;

	default:
		return false;
	}
}

bool isValidFrame(const CanFrame& frame)
{
	if (frame.length < 1 || frame.length > 8)
		return false;

	if (!isValidNode(frame.id))
		return false;

	if (!isValidCommand(frame.data[0]))
		return false;

	return true;
}

std::string commandToString(const Command& cmd)
{
	std::ostringstream out;

	out << "\nNode: " << toString(cmd.node)
		<< "\nCommand: " << toString(cmd.command)
		<< "\nData Length: " << static_cast<int>(cmd.dataLength)
		<< "\nData: ";

	for (uint8_t i = 0; i < cmd.dataLength; ++i)
	{
		out << static_cast<int>(cmd.data[i]) << " ";
	}

	return out.str();
}

std::string_view toString(NodeId id)
{
	switch (id)
	{
	case NodeId::Head:
		return "Head";

	case NodeId::Cooling:
		return "Cooling";

	case NodeId::Arms:
		return "Arms";

	case NodeId::Base:
		return "Base";

	default:
		return "Unknown";
	}
}
std::string_view toString(CommandId id)
{
	switch (id)
	{
	case CommandId::Set:
		return "Set";

	case CommandId::Stop:
		return "Stop";

	case CommandId::GetStatus:
		return "GetStatus";

	case CommandId::Status:
		return "Status";

	default:
		return "Unknown";
	}
}

std::string frameToString(const CanFrame& frame)
{
	std::ostringstream out;

	out << "\nFrame ID: " << frame.id
		<< "\nFrame Length: " << static_cast<int>(frame.length)
		<< "\nFrame Data: ";

	for (uint8_t i = 0; i < frame.length; ++i)
	{
		out << std::hex
			<< std::uppercase
			<< std::setw(2)
			<< std::setfill('0')
			<< static_cast<int>(frame.data[i])
			<< " ";
	}

	return out.str();
}
