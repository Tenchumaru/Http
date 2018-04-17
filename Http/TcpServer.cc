#include "stdafx.h"
#include "TcpServer.h"
#include "Waiter.h"

TcpServer::TcpServer(fn_t onConnect) : onConnect(onConnect) {}

TcpServer::~TcpServer() {}

void TcpServer::Listen(unsigned short port) {
	char buf[6];
	sprintf_s(buf, "%d", port);
	Listen(buf);
}

void TcpServer::Listen(char const* service) {
#ifdef _WIN32
	// Initialize the socket library.
	static bool isInitialized= [] {
		WSADATA wsaData;
		auto result= WSAStartup(MAKEWORD(2, 2), &wsaData);
		if(result) {
			std::cerr << "WSAStartup error " << result << std::endl;
			throw std::runtime_error("TcpServer::Listen.WSAStartup");
		}
		return true;
	}();
#endif

	// Create and bind the server socket.
	addrinfo hints{};
	hints.ai_family= AF_UNSPEC; // Allow IPv4 or IPv6.
	hints.ai_socktype= SOCK_STREAM;
	hints.ai_flags= AI_PASSIVE; // Use INADDR_ANY for the resulting addresses.
	addrinfo* addresses;
	if(getaddrinfo(nullptr, service, &hints, &addresses)) {
		throw std::runtime_error("TcpServer::Listen.getaddrinfo");
	}
	SOCKET server= INVALID_SOCKET;
	socklen_t sock_addr_size= 0;
	try {
		for(auto const* rp= addresses; rp != nullptr; rp= rp->ai_next) {
			server= socket(rp->ai_family, rp->ai_socktype, rp->ai_protocol);
			if(server == INVALID_SOCKET) {
				continue;
			}
			int reuse= 1;
			if(setsockopt(server, SOL_SOCKET, SO_REUSEADDR, reinterpret_cast<char const*>(&reuse), sizeof(reuse)) < 0) {
				throw std::runtime_error("TcpServer::Listen.setsockopt(SO_REUSEADDR)");
			}
#ifdef SO_REUSEPORT
			if(setsockopt(server, SOL_SOCKET, SO_REUSEPORT, &reuse, sizeof(reuse)) < 0) {
				throw std::runtime_error("TcpServer::Listen.setsockopt(SO_REUSEPORT)");
			}
#endif
			sock_addr_size= static_cast<int>(rp->ai_addrlen);
			if(bind(server, rp->ai_addr, sock_addr_size) == 0) {
				break;
			}
			close(server);
		}
		freeaddrinfo(addresses);
	} catch(std::exception const&) {
		freeaddrinfo(addresses);
		throw;
	}
	if(server == INVALID_SOCKET) {
		throw std::runtime_error("TcpServer::Listen.socket");
	}

	// Listen for client connections.
	check(listen(server, 1));
	Accept(server, sock_addr_size);
}

void TcpServer::Accept(SOCKET server, socklen_t sock_addr_size) {
	try {
		// Await connections and requests.
		union {
			sockaddr_in ipv4;
			sockaddr_in6 ipv6;
		} u{};
		auto* p= reinterpret_cast<sockaddr*>(&u);
		for(;;) {
			SOCKET client= accept(server, p, &sock_addr_size);
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
			static_assert(sizeof(uint64_t) >= sizeof(SOCKET), "unexpected size");

			// This is a new client connection.  Handle it.
			onConnect(Socket(client));
		}
	} catch(std::exception& ex) {
		std::cout << "exception: " << ex.what() << std::endl;
	}

	// Close the server socket and exit.
	close(server);
}
