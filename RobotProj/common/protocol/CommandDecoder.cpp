#include "CommandDecoder.hpp"
#include <cstddef>

DecodeResult CommandDecoder::push(const CanFrame& frame)
{
	Command cmd{};
	if (!isValidFrame(frame))
		return { DecodeStatus::Rejected,std::nullopt };

	const CanIdFields id = decodeCanId(frame.id);
	if (id.type == FrameType::Single)
	{
		CommandId command =
			static_cast<CommandId>(frame.data[0]);

		if (!isValidCommand(command))
			return { DecodeStatus::Rejected,std::nullopt };

		cmd.node = id.node;
		cmd.command = command;

		cmd.data.resize(frame.length - 1);
		for (std::size_t i = 0; i < cmd.data.size(); ++i)
		{
			cmd.data[i] = frame.data[i + 1];
		}

		return { DecodeStatus::Complete, cmd };
	}


	if (id.type == FrameType::SequenceStart)
	{
		if (frame.length != 8)
			return { DecodeStatus::Rejected, std::nullopt };

		const CommandId command = static_cast<CommandId>(frame.data[0]);

		if (!isValidCommand(command))
			return { DecodeStatus::Rejected, std::nullopt };

		const std::size_t length = (static_cast<std::size_t>(frame.data[2]) << 8) | frame.data[3];

		if (length < 8 || length>1534)
			return{ DecodeStatus::Rejected,std::nullopt };

		pending_.emplace();
		Pending& pen = *pending_;
		pen.currentCommand_.node = id.node;
		pen.currentCommand_.command = command;
		pen.sequenceId_ = frame.data[1];
		pen.expectedLength_ = length;
		pen.nextIndex_ = 1;

		for (std::size_t i = 4; i < frame.length; ++i)
		{
			pen.currentCommand_.data.push_back(frame.data[i]);
		}

		return { DecodeStatus::InProgress, std::nullopt };
	}
	else if (id.type == FrameType::SequenceData)
	{
		if (!pending_.has_value())
			return { DecodeStatus::Rejected, std::nullopt };

		Pending& pen = *pending_;
		if (id.node != pen.currentCommand_.node)
			return { DecodeStatus::Rejected, std::nullopt };

		
		if (frame.length != 8)
			return { DecodeStatus::Rejected, std::nullopt };
		if (pen.sequenceId_ != frame.data[0])
			return { DecodeStatus::Rejected, std::nullopt };
		if (pen.nextIndex_ != (frame.data[1]))
			return { DecodeStatus::Rejected, std::nullopt };

		const std::size_t incomingBytes = frame.length - 2;

		if (pen.currentCommand_.data.size() + incomingBytes > pen.expectedLength_)
		{
			pending_.reset();
			return { DecodeStatus::Rejected, std::nullopt };
		}
		for (std::size_t i = 2; i < frame.length; ++i)
		{
			pen.currentCommand_.data.push_back(frame.data[i]);
		}
		++pen.nextIndex_;
		return { DecodeStatus::InProgress, std::nullopt };
	}
	else if (id.type == FrameType::SequenceEnd)
	{
		if (!pending_.has_value())
			return { DecodeStatus::Rejected, std::nullopt };

		Pending& pen = *pending_;
		if (id.node != pen.currentCommand_.node)
			return { DecodeStatus::Rejected, std::nullopt };
		if (frame.length < 3)
			return { DecodeStatus::Rejected, std::nullopt };
		if (pen.sequenceId_ != frame.data[0])
			return { DecodeStatus::Rejected, std::nullopt };
		if (pen.nextIndex_ != frame.data[1] )
			return { DecodeStatus::Rejected, std::nullopt };
		
		const std::size_t incomingBytes = frame.length - 2;

		if (pen.currentCommand_.data.size() + incomingBytes > pen.expectedLength_)
		{
			pending_.reset();
			return { DecodeStatus::Rejected, std::nullopt };
		}
		for (std::size_t i = 2; i < frame.length; ++i)
		{
			pen.currentCommand_.data.push_back(frame.data[i]);
		}
		if (pen.currentCommand_.data.size() != pen.expectedLength_) {
			pending_.reset();
			return{ DecodeStatus::Rejected, std::nullopt };
		}
		cmd = pen.currentCommand_;
		pending_.reset();
		return { DecodeStatus::Complete, cmd };
	}
	return { DecodeStatus::Rejected, std::nullopt };
}


