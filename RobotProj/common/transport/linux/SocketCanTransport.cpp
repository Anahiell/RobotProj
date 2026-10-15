#include "SocketCanTransport.hpp"

#include <unistd.h>
#include <sys/socket.h>
#include <linux/can.h>
#include <linux/can/raw.h>
#include <net/if.h>
#include <algorithm>

SocketCanTransport::~SocketCanTransport()
{
	close();
}
void SocketCanTransport::close() noexcept 
{
	if (socketFd_ >= 0)
	{
		::close(socketFd_);
		socketFd_ = -1;
	}
}
bool SocketCanTransport::open(const std::string& interfaceName)
{
	close();
	sockaddr_can address{};
	socketFd_ = ::socket(PF_CAN, SOCK_RAW, CAN_RAW);
	if (socketFd_ >= 0) {
		const unsigned int interfaceIndex = ::if_nametoindex(interfaceName.c_str());
		if (interfaceIndex == 0)
		{
			close();
			return false;
		}
		address.can_family = AF_CAN;
		address.can_ifindex = static_cast<int>(interfaceIndex);

		if (::bind(socketFd_, reinterpret_cast<const sockaddr*>(&address), sizeof(address)) < 0)
		{
			close();
			return false;
		}
		else
			return true;
	}
	else {
		close();
		return false;
	}
}

bool SocketCanTransport::send(const CanFrame& frame)
{
	if (socketFd_ < 0)
		return false;

	struct can_frame raw {};
	if (frame.length > 8 || frame.id > CAN_SFF_MASK)
		return false;

	raw.can_id = frame.id;
	raw.len = frame.length;
	std::copy_n(frame.data.begin(), frame.length, raw.data);
	ssize_t written = ::write(socketFd_, &raw, sizeof(raw));

	return written == static_cast<ssize_t>(sizeof(raw));
}

bool SocketCanTransport::receive(CanFrame& frame)
{
	if (socketFd_ < 0)
		return false;

	struct can_frame raw {};
	
	const ssize_t bytesRead = ::read(socketFd_,&raw,sizeof(raw));
	if (bytesRead != static_cast<ssize_t>(sizeof(raw)))
		return false;

	if (raw.len >8)
		return false;
	if (raw.can_id & (CAN_EFF_FLAG | CAN_RTR_FLAG | CAN_ERR_FLAG))
		return false;

	frame.id = raw.can_id & CAN_SFF_MASK;
	frame.length = raw.len;
	std::copy_n(raw.data, raw.len, frame.data.begin());

	return true;
}
