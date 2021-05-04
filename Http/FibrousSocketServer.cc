#include "pch.h"
#include "TcpSocket.h"
#include "FibrousSocketServer.h"
#ifdef _WIN32
using FiberFn = LPFIBER_START_ROUTINE;
#else
# include "Fiber.h"
#endif

namespace {
	constexpr size_t stackSize = 0x1'0000;

#ifdef _WIN32
	inline int poll(_Inout_ LPWSAPOLLFD fdArray, _In_ size_t fds, _In_ INT timeout) {
		return WSAPoll(fdArray, static_cast<ULONG>(fds), timeout);
	}
#endif
}

using namespace std::literals;

std::pair<SOCKET, socklen_t> FibrousSocketServer::Open(char const* service) {
	mainFiber = ConvertThreadToFiber(this);
	if (!mainFiber) {
		throw std::runtime_error("ConvertThreadToFiber");
	}
	auto [serverSocket, addressSize] = SocketServer::Open(service);
	if (!Socket::SetNonblocking(serverSocket)) {
		throw std::runtime_error("FibrousSocketServer::Open.Socket::SetNonblocking");
	}
	awaitFn = [this](SOCKET clientSocket, short pollValue) { Await(clientSocket, pollValue); };
	currentAwaits.fibers.push_back(mainFiber);
	currentAwaits.polls.push_back({ serverSocket, POLLIN });
	nextAwaits.fibers.push_back(mainFiber);
	nextAwaits.polls.push_back({ serverSocket, POLLIN });
	return { serverSocket, addressSize };
}

std::pair<SOCKET, int> FibrousSocketServer::Accept(SOCKET serverSocket, socklen_t addressSize) {
	for (;;) {
		currentAwaits.fibers.resize(1);
		currentAwaits.polls.resize(1);
		std::swap(currentAwaits, nextAwaits);
		check(poll(currentAwaits.polls.data(), currentAwaits.polls.size(), -1));
		for (size_t i = 1, n = currentAwaits.polls.size(); i < n; ++i) {
			auto* const currentFiber = currentAwaits.fibers[i];
			auto const& currentPoll = currentAwaits.polls[i];
			if (currentPoll.revents) {
				SwitchToFiber(currentFiber);
			} else {
				nextAwaits.fibers.push_back(currentFiber);
				nextAwaits.polls.push_back(currentPoll);
			}
		}
		if (currentAwaits.polls[0].revents & POLLIN) {
			auto [clientSocket, errorCode] = SocketServer::Accept(serverSocket, addressSize);
			if (errorCode) {
				std::cerr << "SocketServer::Accept failed, error " << errorCode << std::endl;
			} else {
				return{ clientSocket, errorCode };
			}
		} else if (currentAwaits.polls[0].revents) {
			std::cerr << "server socket poll failed, error " << errno << std::endl;
		}
	}
}

int FibrousSocketServer::ConnectImpl(SOCKET clientSocket, sockaddr const* address, size_t addressSize) noexcept {
	for (;;) {
		int result = SocketServer::ConnectImpl(clientSocket, address, addressSize);
		if (FibrousTcpSocket::IsAwaitable(result)) {
			Await(clientSocket, POLLOUT);
		} else {
			return result;
		}
	}
}

void FibrousSocketServer::Handle(SOCKET clientSocket) {
	void* fiber;
	if (availableFibers.empty()) {
		fiber = CreateFiber(stackSize, &FibrousSocketServer::RunFiber, this);
		if (!fiber) {
			std::cerr << "CreateFiber failed: " << errno << std::endl;
			return;
		}
	} else {
		fiber = availableFibers.back();
		availableFibers.pop_back();
	}
	fiberSocket = std::make_unique<FibrousTcpSocket>(clientSocket, awaitFn);
	SwitchToFiber(fiber);
}

void WINAPI FibrousSocketServer::RunFiber(void* parameter) {
	auto* p = reinterpret_cast<FibrousSocketServer*>(parameter);
	for (;;) {
		try {
			p->InternalHandle(std::move(p->fiberSocket));
		} catch (std::exception const& ex) {
			std::cout << "FibrousSocketServer::RunFiber.InternalHandle:  " << ex.what() << std::endl;
		}
		p->availableFibers.push_back(GetCurrentFiber());
		SwitchToFiber(p->mainFiber);
	}
}

void FibrousSocketServer::Await(SOCKET clientSocket, short pollValue) {
	nextAwaits.fibers.push_back(GetCurrentFiber());
	nextAwaits.polls.push_back({ clientSocket, pollValue });
	SwitchToFiber(mainFiber);
}
