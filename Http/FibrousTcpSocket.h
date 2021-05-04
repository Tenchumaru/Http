#pragma once

#include "TcpSocket.h"

class FibrousTcpSocket : public TcpSocket {
public:
	using fn_t = std::function<void(SOCKET, short)>;

	FibrousTcpSocket(SOCKET socket, fn_t awaitFn) : TcpSocket(socket), awaitFn(awaitFn) {}
	FibrousTcpSocket() = delete;
	FibrousTcpSocket(FibrousTcpSocket const&) = delete;
	FibrousTcpSocket(FibrousTcpSocket&& that) noexcept : TcpSocket(std::move(that)), awaitFn(that.awaitFn) {}
	FibrousTcpSocket& operator=(FibrousTcpSocket const&) = delete;
	FibrousTcpSocket& operator=(FibrousTcpSocket&& that) noexcept;
	~FibrousTcpSocket() = default;

protected:
	int InternalReceive(char* buffer, size_t bufferSize) override;
	int InternalSend(char const* buffer, size_t bufferSize) override;

private:
	fn_t awaitFn;
	bool IsAwaiting(int result, short pollValue);
};
