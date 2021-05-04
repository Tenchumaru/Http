#pragma once

#include "SocketServer.h"
#include "FibrousTcpSocket.h"

class FibrousSocketServer : public SocketServer {
public:
	FibrousSocketServer() = default;
	FibrousSocketServer(FibrousSocketServer const&) = delete;
	FibrousSocketServer(FibrousSocketServer&&) noexcept = default;
	FibrousSocketServer& operator=(FibrousSocketServer const&) = delete;
	FibrousSocketServer& operator=(FibrousSocketServer&&) noexcept = default;
	~FibrousSocketServer() override = default;

protected:
	std::pair<SOCKET, socklen_t> Open(char const* service) override;
	std::pair<SOCKET, int> Accept(SOCKET serverSocket, socklen_t addressSize) override;
	int ConnectImpl(SOCKET clientSocket, sockaddr const* address, size_t addressSize) noexcept override;

private:
	struct {
		std::vector<void*> fibers;
		std::vector<pollfd> polls;
	} currentAwaits, nextAwaits;
	std::vector<void*> availableFibers;
	std::unique_ptr<FibrousTcpSocket> fiberSocket;
	FibrousTcpSocket::fn_t awaitFn;
	void* mainFiber;

	void Handle(SOCKET clientSocket) override;
	static void WINAPI RunFiber(void* parameter);
	virtual void InternalHandle(std::unique_ptr<FibrousTcpSocket> clientSocket) = 0;
	void Await(SOCKET clientSocket, short pollValue);
};
