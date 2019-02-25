#include "stdafx.h"
#include "FibrousTcpSocketFactory.h"
#include "FibrousTcpSocket.h"
#include "Waiter.h"
#ifdef _WIN32
using FiberFn = LPFIBER_START_ROUTINE;
#else
# include "Fiber.h"
#endif

namespace {
	constexpr size_t stackSize = 0x10000;

	void set_nonblocking(SOCKET s) {
#ifdef _WIN32
		u_long value = 1;
		auto const result = ioctlsocket(s, FIONBIO, &value);
		if(result != 0) {
			std::cout << "set_nonblocking ioctlsocket failure: " << errno << std::endl;
			throw std::runtime_error("set_nonblocking ioctlsocket failure");
		}
#else
		auto result = fcntl(s, F_GETFL, 0);
		if(result >= 0) {
			result |= O_NONBLOCK;
			result = fcntl(s, F_SETFL, result);
		}
		if(result != 0) {
			perror("set_nonblocking fcntl failure");
			throw std::runtime_error("set_nonblocking fcntl failure");
		}
#endif
	}
}

FibrousTcpSocketFactory::FibrousTcpSocketFactory() {}

FibrousTcpSocketFactory::~FibrousTcpSocketFactory() {}

void FibrousTcpSocketFactory::Accept(SOCKET server, socklen_t addressSize, fn_t onConnect_) {
	// Enable the server socket for asynchronous connections.
	set_nonblocking(server);
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
		SOCKADDR_STORAGE address;
		auto* p = reinterpret_cast<sockaddr*>(&address);
		for(;;) {
			SOCKET ready = waiter.Wait();
			if(ready == server) {
				client = accept(server, p, &addressSize);
				if(client == INVALID_SOCKET) {
					// Assume the failure is due to the network infrastructure
					// rejecting the connection.
					std::cout << "failed connection: " << errno << std::endl;
					continue;
				}
#ifdef _DEBUG
				char node[NI_MAXHOST], service[NI_MAXSERV];
				if(getnameinfo(p, addressSize, node, sizeof(node), service, sizeof(service), NI_NUMERICHOST | NI_NUMERICSERV) == 0) {
					std::cout << "new connection: " << client << '@' << node << ':' << service << std::endl;
				} else {
					std::cerr << "cannot get name information for accepted socket of family " << address.ss_family << std::endl;
				}
#endif
				set_nonblocking(client);

				// Create a fiber and invoke the handler on it.
				void* fiber;
				if(availableFibers.empty()) {
					fiber = CreateFiber(stackSize, &FibrousTcpSocketFactory::InvokeOnConnect, this);
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

void FibrousTcpSocketFactory::InvokeOnConnect(void* parameter) {
	auto* p = reinterpret_cast<FibrousTcpSocketFactory*>(parameter);
	for(;;) {
		p->onConnect(FibrousTcpSocket(p->client, p->Await));
		p->availableFibers.push_back(GetCurrentFiber());
		SwitchToFiber(p->mainFiber);
	}
}
