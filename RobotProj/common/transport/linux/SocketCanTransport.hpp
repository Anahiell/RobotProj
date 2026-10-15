#pragma once

#include <string>
#include "RobotProtocol.hpp"

class SocketCanTransport
{
public:
	SocketCanTransport() = default;
	SocketCanTransport(const SocketCanTransport&) = delete;
	SocketCanTransport& operator=(const SocketCanTransport&) = delete;
	~SocketCanTransport();

	bool open(const std::string& interfaceName);
	void close() noexcept;

	bool send(const CanFrame& frame);
	bool receive(CanFrame& frame);

private:
	int socketFd_ = -1; // -1 socket not yet open
};
