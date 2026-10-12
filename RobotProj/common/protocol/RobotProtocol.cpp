#include "RobotProtocol.hpp"

#include <iomanip>
#include <sstream>
#include <vector>


// ============================================================
// Primitive codecs
// ============================================================

std::array<uint8_t, 2> encodeInt16(int16_t value)
{
	uint16_t raw = static_cast<uint16_t>(value);

	return
	{
		static_cast<uint8_t>(raw >> 8),
		static_cast<uint8_t>(raw & 0xFF)
	};
}


int16_t decodeInt16(std::array<uint8_t, 2> data)
{
	uint16_t raw =
		(static_cast<uint16_t>(data[0]) << 8) |
		static_cast<uint16_t>(data[1]);

	return static_cast<int16_t>(raw);
}


std::array<uint8_t, 4> encodeInt32(int32_t value)
{
	uint32_t raw = static_cast<uint32_t>(value);

	return
	{
		static_cast<uint8_t>((raw >> 24) & 0xFF),
		static_cast<uint8_t>((raw >> 16) & 0xFF),
		static_cast<uint8_t>((raw >> 8) & 0xFF),
		static_cast<uint8_t>(raw & 0xFF)
	};
}


int32_t decodeInt32(std::array<uint8_t, 4> data)
{
	uint32_t raw =
		(static_cast<uint32_t>(data[0]) << 24) |
		(static_cast<uint32_t>(data[1]) << 16) |
		(static_cast<uint32_t>(data[2]) << 8) |
		static_cast<uint32_t>(data[3]);

	return static_cast<int32_t>(raw);
}


// ============================================================
// CAN ID
//
// [10:8] priority
// [7:3]  node
// [2:0]  frame type
// ============================================================

uint32_t encodeCanId(
	CanPriority priority,
	NodeId node,
	FrameType type)
{
	uint32_t priorityBits =
		static_cast<uint32_t>(priority) & 0x07;

	uint32_t nodeBits =
		static_cast<uint32_t>(node) & 0x1F;

	uint32_t typeBits =
		static_cast<uint32_t>(type) & 0x07;

	return
		(priorityBits << 8) |
		(nodeBits << 3) |
		typeBits;
}


CanIdFields decodeCanId(uint32_t id)
{
	CanIdFields fields{};

	fields.priority =
		static_cast<CanPriority>((id >> 8) & 0x07);

	fields.node =
		static_cast<NodeId>((id >> 3) & 0x1F);

	fields.type =
		static_cast<FrameType>(id & 0x07);

	return fields;
}


// ============================================================
// Command <-> CAN
// ============================================================

std::vector<CanFrame> encodeCommand(
	const Command& cmd,
	CanPriority priority)
{
	if (cmd.dataLength > cmd.data.size())
		return {};

	std::vector<CanFrame> frames{};
	CanFrame frame{};

	size_t sourceOffset = 0;
	uint8_t sequenceId = 1; //TODO: Временно для тестирования
	if (cmd.dataLength > 7)
	{
		size_t count_frames = 1 + (cmd.dataLength - 4 + 5) / 6;
		for (size_t i = 0; i < count_frames; i++)
		{
			frame = CanFrame{};
			//DATA START
			if (i == 0) {
				frame.id = encodeCanId(
					priority,
					cmd.node,
					FrameType::SequenceStart
				);
				frame.data[0] = static_cast<uint8_t>(cmd.command);
				uint16_t length = cmd.dataLength;

				frame.data[1] = sequenceId; 
				frame.data[2] = static_cast<uint8_t>(length >> 8);
				frame.data[3] = static_cast<uint8_t>(length & 0xFF);
				frame.length = 4;
			}
			//DATA END
			else if (i == count_frames-1)
			{
				frame.id = encodeCanId(
					priority,
					cmd.node,
					FrameType::SequenceEnd
				);
				frame.data[0] = sequenceId;
				frame.data[1] = static_cast<uint8_t>(i);
				frame.length = 2; // Data / End
			}
			//DATA
			else {
				frame.id = encodeCanId(
					priority,
					cmd.node,
					FrameType::SequenceData
				);
				frame.data[0] = sequenceId;
				frame.data[1] = static_cast<uint8_t>(i);
				frame.length = 2; // Data / End
			}
			
			while (frame.length < 8 && sourceOffset < cmd.dataLength)
			{
				frame.data[frame.length] = cmd.data[sourceOffset];

				++frame.length;
				++sourceOffset;
			}
			frames.push_back(frame);
		}
	}
	else {
		frame.id = encodeCanId(
			priority,
			cmd.node,
			FrameType::Single
		);

		frame.data[0] =
			static_cast<uint8_t>(cmd.command);

		for (uint8_t i = 0; i < cmd.dataLength; ++i)
		{
			frame.data[i + 1] = cmd.data[i];
		}

		frame.length = cmd.dataLength + 1;
		frames.push_back(frame);
	}
	return frames;
}


Command decodeCommand(const CanFrame& frame)
{
	Command cmd{};

	if (!isValidFrame(frame))
		return cmd;

	CanIdFields id = decodeCanId(frame.id);

	// Этот decoder пока только для Single.
	if (id.type != FrameType::Single)
		return cmd;

	if (frame.length < 1)
		return cmd;

	CommandId command =
		static_cast<CommandId>(frame.data[0]);

	if (!isValidCommand(command))
		return cmd;

	cmd.node = id.node;
	cmd.command = command;

	cmd.dataLength = frame.length - 1;

	for (uint8_t i = 0; i < cmd.dataLength; ++i)
	{
		cmd.data[i] = frame.data[i + 1];
	}

	return cmd;
}


// ============================================================
// Validation
// ============================================================

bool isValidNode(NodeId node)
{
	switch (node)
	{
	case NodeId::Broadcast:
	case NodeId::Head:
	case NodeId::Cooling:
	case NodeId::Arms:
	case NodeId::Base:
		return true;

	default:
		return false;
	}
}


bool isValidCommand(CommandId command)
{
	switch (command)
	{
	case CommandId::Set:
	case CommandId::Stop:
	case CommandId::GetStatus:
	case CommandId::Status:
	case CommandId::Heartbeat:
		return true;

	default:
		return false;
	}
}


bool isValidFrameType(FrameType type)
{
	switch (type)
	{
	case FrameType::Single:
	case FrameType::SequenceStart:
	case FrameType::SequenceData:
	case FrameType::SequenceEnd:
	case FrameType::Ack:
		return true;

	default:
		return false;
	}
}


bool isValidFrame(const CanFrame& frame)
{
	// Standard 11-bit CAN ID.
	if (frame.id > 0x7FF)
		return false;

	if (frame.length > 8)
		return false;

	CanIdFields fields = decodeCanId(frame.id);

	if (!isValidNode(fields.node))
		return false;

	if (!isValidFrameType(fields.type))
		return false;

	// Для Single первый DATA byte обязан быть CommandId.
	if (fields.type == FrameType::Single)
	{
		if (frame.length < 1)
			return false;

		CommandId command =
			static_cast<CommandId>(frame.data[0]);

		if (!isValidCommand(command))
			return false;
	}

	return true;
}


// ============================================================
// Debug
// ============================================================

std::string commandToString(const Command& cmd)
{
	std::ostringstream out;

	out << "\nNode: " << toString(cmd.node)
		<< "\nCommand: " << toString(cmd.command)
		<< "\nData Length: "
		<< static_cast<int>(cmd.dataLength)
		<< "\nData: ";

	for (uint8_t i = 0; i < cmd.dataLength; ++i)
	{
		out << std::hex
			<< std::uppercase
			<< std::setw(2)
			<< std::setfill('0')
			<< static_cast<int>(cmd.data[i])
			<< " ";
	}

	return out.str();
}


std::string frameToString(const CanFrame& frame)
{
	std::ostringstream out;

	CanIdFields fields = decodeCanId(frame.id);

	out << "\nCAN ID: 0x"
		<< std::hex
		<< std::uppercase
		<< frame.id

		<< "\nPriority: "
		<< std::dec
		<< static_cast<int>(fields.priority)

		<< "\nNode: "
		<< toString(fields.node)

		<< "\nFrame Type: "
		<< toString(fields.type)

		<< "\nLength: "
		<< static_cast<int>(frame.length)

		<< "\nData: ";

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


std::string_view toString(NodeId id)
{
	switch (id)
	{
	case NodeId::Broadcast:
		return "Broadcast";

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

	case CommandId::Heartbeat:
		return "Heartbeat";

	default:
		return "Unknown";
	}
}


std::string_view toString(FrameType type)
{
	switch (type)
	{
	case FrameType::Single:
		return "Single";

	case FrameType::SequenceStart:
		return "SequenceStart";

	case FrameType::SequenceData:
		return "SequenceData";

	case FrameType::SequenceEnd:
		return "SequenceEnd";

	case FrameType::Ack:
		return "Ack";

	default:
		return "Unknown";
	}
}