#pragma once

#include <optional>
#include <cstddef>

#include "RobotProtocol.hpp"

enum class DecodeStatus
{
	InProgress,
	Complete,
	Rejected
};

struct DecodeResult
{
	DecodeStatus dStatus;

	std::optional<Command> cmd;
};

class CommandDecoder
{	
public:
	DecodeResult push(const CanFrame& frame);
private:
	struct Pending
	{
		Command currentCommand_{};
		std::size_t expectedLength_ = 0;
		uint8_t sequenceId_ = 0;
		uint8_t nextIndex_ = 1;
	};
	std::optional<Pending> pending_;

};
