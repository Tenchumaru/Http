#pragma once

#include "TcpSocket.h"

class FibrousTcpSocket : public TcpSocket {
public:
	using fn_t = std::function<void(SOCKET, short)>;

	FibrousTcpSocket(SOCKET socket, fn_t Await) : TcpSocket(socket), Await(Await) {}
	FibrousTcpSocket() = delete;
	FibrousTcpSocket(FibrousTcpSocket const&) = delete;
	FibrousTcpSocket(FibrousTcpSocket&&) = default;
	FibrousTcpSocket& operator=(FibrousTcpSocket const&) = delete;
	FibrousTcpSocket& operator=(FibrousTcpSocket&&) = default;
	~FibrousTcpSocket() = default;

protected:
	int InternalReceive(char* buffer, size_t bufferSize) override;
	int InternalSend(char const* buffer, size_t bufferSize) override;

private:
	fn_t Await;
	bool IsAwaiting(int result, short pollValue);
};
