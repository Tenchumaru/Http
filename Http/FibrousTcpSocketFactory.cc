#include "stdafx.h"
#include "FibrousTcpSocketFactory.h"
#include "FibrousTcpSocket.h"
#include "Waiter.h"

static size_t const stackSize= 8 * 1024;

FibrousTcpSocketFactory::FibrousTcpSocketFactory() {}

FibrousTcpSocketFactory::~FibrousTcpSocketFactory() {}

void FibrousTcpSocketFactory::Accept(SOCKET server, socklen_t sock_addr_size, fn_t onConnect_) {
	// Enable the server socket for asynchronous connections.
	Waiter waiter;
	waiter.Add(server, POLLIN);

	// Initialize for coroutines.
	mainFiber= ConvertThreadToFiber(this);

	try {
		// Await connections and requests.
		union {
			sockaddr_in ipv4;
			sockaddr_in6 ipv6;
		} u{};
		auto* p= reinterpret_cast<sockaddr*>(&u);
		for(;;) {
			SOCKET ready= waiter.Wait();
			if(ready == server) {
				client= accept(server, p, &sock_addr_size);
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
				onConnect= onConnect_;
				auto fiber= CreateFiber(stackSize, InvokeOnConnect, this);
				// TODO:  save the fiber.
				SwitchToFiber(fiber);
			}
		}
	} catch(std::exception& ex) {
		std::cout << "exception: " << ex.what() << std::endl;
	}

	// Close the server socket and exit.
	close(server);
}

void FibrousTcpSocketFactory::InvokeOnConnect(void* parameter) {
	auto* p= reinterpret_cast<FibrousTcpSocketFactory*>(parameter);
	p->onConnect(FibrousTcpSocket(p->client));
}
