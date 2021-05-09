#pragma once

#include "TcpSocket.h"

class SocketServer {
public:
	SocketServer() = default;
	SocketServer(SocketServer const&) = delete;
	SocketServer(SocketServer&&) noexcept = default;
	SocketServer& operator=(SocketServer const&) = delete;
	SocketServer& operator=(SocketServer&&) noexcept = default;
	virtual ~SocketServer() = default;
	void Run(char const* service);
	void Run(unsigned short port);
	std::unique_ptr<TcpSocket> Connect(char const* nodeName, char const* serviceName);
	std::unique_ptr<TcpSocket> Connect(char const* host, std::uint16_t port);

protected:
	virtual std::pair<SOCKET, socklen_t> Open(char const* service);
	virtual std::pair<SOCKET, int> Accept(SOCKET serverSocket, socklen_t addressSize);
	virtual std::unique_ptr<TcpSocket> MakeSocketImpl(SOCKET socket) const;

private:
	virtual void Handle(SOCKET clientSocket) = 0;
	virtual bool TrySocketUse(SOCKET serverSocket, addrinfo const* address, socklen_t& addressSize);
};
