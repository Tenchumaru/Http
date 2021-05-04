#pragma once

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
	SOCKET Connect(char const* nodeName, char const* serviceName);
	SOCKET Connect(char const* host, std::uint16_t port);

protected:
	virtual std::pair<SOCKET, socklen_t> Open(char const* service);
	virtual std::pair<SOCKET, int> Accept(SOCKET serverSocket, socklen_t addressSize);
	virtual int ConnectImpl(SOCKET clientSocket, sockaddr const* address, size_t addressSize) noexcept;

private:
	virtual void Handle(SOCKET clientSocket) = 0;
};
