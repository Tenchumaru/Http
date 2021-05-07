#pragma once

#include "TcpSocket.h"

class FibrousTcpSocket : public TcpSocket {
public:
	using fn_t = std::function<void(SOCKET, short)>;

	FibrousTcpSocket(SOCKET socket, fn_t awaitFn);
	FibrousTcpSocket() = delete;
	FibrousTcpSocket(FibrousTcpSocket const&) = delete;
	FibrousTcpSocket(FibrousTcpSocket&& that) noexcept : TcpSocket(std::move(that)), awaitFn(that.awaitFn) {}
	FibrousTcpSocket& operator=(FibrousTcpSocket const&) = delete;
	FibrousTcpSocket& operator=(FibrousTcpSocket&& that) noexcept;
	~FibrousTcpSocket() = default;
	static bool IsAwaitable(int errorValue);
	int Connect(sockaddr const* address, size_t addressSize) noexcept override;

protected:
	void Await(short pollValue);
	int InternalReceive(char* buffer, size_t bufferSize) override;
	int InternalSend(char const* buffer, size_t bufferSize) override;

private:
	fn_t awaitFn;

	bool IsAwaiting(int result, short pollValue);
};
