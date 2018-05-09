#include "stdafx.h"
#include "FibrousTcpSocketFactory.h"
#include "FibrousTcpSocket.h"
#include "Waiter.h"

namespace {
	size_t const stackSize = 0x10000;

#ifndef _WIN32
	void* ConvertThreadToFiber(void*) {
		std::terminate();
	}

	void* CreateFiber(size_t, void(*)(void*), void*) {
		std::terminate();
	}

	void* SwitchToFiber(void*) {
		std::terminate();
	}
#endif
}

FibrousTcpSocketFactory::FibrousTcpSocketFactory() {}

FibrousTcpSocketFactory::~FibrousTcpSocketFactory() {}

void FibrousTcpSocketFactory::Accept(SOCKET server, socklen_t sock_addr_size, fn_t onConnect_) {
	// Enable the server socket for asynchronous connections.
	onConnect = onConnect_;
	Waiter waiter;
	waiter.Add(server, POLLIN);

	// Create the await function.
	std::unordered_map<SOCKET, void*> fiberMap;
	Await = [this, &waiter, &fiberMap](SOCKET s, short pollValue) {
		waiter.Add(s, pollValue);
		fiberMap[s] = GetCurrentFiber();
		SwitchToFiber(mainFiber);
		fiberMap.erase(s);
	};

	// Initialize for coroutines.
	mainFiber = ConvertThreadToFiber(this);
	if(!mainFiber) {
		throw std::runtime_error("ConvertThreadToFiber");
	}

	try {
		// Await connections and requests.
		union {
			sockaddr_in ipv4;
			sockaddr_in6 ipv6;
		} u{};
		auto* p = reinterpret_cast<sockaddr*>(&u);
		for(;;) {
			SOCKET ready = waiter.Wait();
			if(ready == server) {
				client = accept(server, p, &sock_addr_size);
				if(client == INVALID_SOCKET) {
					// Assume the failure is due to the network infrastructure
					// rejecting the connection.
					std::cout << "failed connection: " << errno << std::endl;
					continue;
				}
#ifdef _DEBUG
				if(sock_addr_size == 16) {
					std::cout << "new connection: " << client << '@' << std::hex << ntohl(u.ipv4.sin_addr.s_addr) << std::dec <<
						':' << ntohs(u.ipv4.sin_port) << std::endl;
				} else {
					std::cout << "new connection: " << client << "@IPv6:" << ntohs(u.ipv6.sin6_port) << std::endl;
				}
#endif

				// Create a fiber and invoke the hander on it.
				void* fiber;
				if(availableFibers.empty()) {
					union {
						decltype(&FibrousTcpSocketFactory::InvokeOnConnect) m;
						LPFIBER_START_ROUTINE p;
					} f{ &FibrousTcpSocketFactory::InvokeOnConnect };
					static_assert(sizeof(f.m) == sizeof(f.p), "unexpected size");
					fiber = CreateFiber(stackSize, f.p, this);
				} else {
					fiber = availableFibers.back();
					availableFibers.pop_back();
				}
				SwitchToFiber(fiber);
				waiter.Add(server, POLLIN);
			} else {
				auto it = fiberMap.find(ready);
				if(it != fiberMap.cend()) {
					auto fiber = it->second;
					fiberMap.erase(it);
					SwitchToFiber(fiber);
				} else {
					std::cout << "warning: abandoned socket" << std::endl;
				}
			}
		}
	} catch(std::exception& ex) {
		std::cout << "exception: " << ex.what() << std::endl;
	}

	// Destroy the fibers, close the server socket, and exit.
	std::for_each(availableFibers.cbegin(), availableFibers.cend(), [](void* fiber) { DeleteFiber(fiber); });
	close(server);
	if(!ConvertFiberToThread()) {
		throw std::runtime_error("ConvertFiberToThread");
	}
}

void FibrousTcpSocketFactory::InvokeOnConnect() {
	for(;;) {
		onConnect(FibrousTcpSocket(client, Await));
		availableFibers.push_back(GetCurrentFiber());
		SwitchToFiber(mainFiber);
	}
}
