#include "CommandEncoder.hpp"
#include <cstddef>

namespace
{
	constexpr std::size_t MAX_SEQUENCE_PAYLOAD = 1534;
}
std::vector<CanFrame> encodeCommand(const Command& cmd, CanPriority priority)
{
	if (cmd.data.size() > MAX_SEQUENCE_PAYLOAD)
		return{};

	std::vector<CanFrame> frames{};
	CanFrame frame{};

	size_t sourceOffset = 0;
	uint8_t sequenceId = 1; //TODO: Временно для тестирования
	if (cmd.data.size() > 7)
	{
		size_t count_frames = 1 + (cmd.data.size() - 4 + 5) / 6;
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
				uint16_t length = static_cast<uint16_t>(cmd.data.size());

				frame.data[1] = sequenceId;
				frame.data[2] = static_cast<uint8_t>(length >> 8);
				frame.data[3] = static_cast<uint8_t>(length & 0xFF);
				frame.length = 4;
			}
			//DATA END
			else if (i == count_frames - 1)
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

			while (frame.length < 8 && sourceOffset < cmd.data.size())
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

		for (std::size_t i = 0; i < cmd.data.size(); ++i)
		{
			frame.data[i + 1] = cmd.data[i];
		}

		frame.length = static_cast<uint8_t>(cmd.data.size() + 1);
		frames.push_back(frame);
	}
	return frames;
}
