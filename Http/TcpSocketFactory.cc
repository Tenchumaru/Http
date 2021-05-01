#include "pch.h"
#include "TcpSocketFactory.h"

void TcpSocketFactory::CreateServer(unsigned short port, fn_t onConnect) {
	char buf[6];
	sprintf_s(buf, "%d", port);
	CreateServer(buf, onConnect);
}

void TcpSocketFactory::CreateServer(char const* service, fn_t onConnect) {
#ifdef _WIN32
	// Initialize the socket library.
	static bool isInitialized = [] {
		WSADATA wsaData;
		auto result = WSAStartup(MAKEWORD(2, 2), &wsaData);
		if (result) {
			std::cerr << "WSAStartup error " << result << std::endl;
			throw std::runtime_error("TcpSocketFactory::CreateServer.WSAStartup");
		}
		return true;
	}();
#endif

	// Create and bind the server socket.
	addrinfo hints{};
	hints.ai_family = AF_UNSPEC; // Allow IPv4 or IPv6.
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE; // Use INADDR_ANY for the resulting addresses.
	addrinfo* addresses;
	if (getaddrinfo(nullptr, service, &hints, &addresses)) {
		throw std::runtime_error("TcpSocketFactory::CreateServer.getaddrinfo");
	}
	SOCKET server = INVALID_SOCKET;
	socklen_t addressSize = 0;
	try {
		for (auto const* rp = addresses; rp != nullptr; rp = rp->ai_next) {
			server = socket(rp->ai_family, rp->ai_socktype, rp->ai_protocol);
			if (server == INVALID_SOCKET) {
				continue;
			}
			int reuse = 1;
			if (setsockopt(server, SOL_SOCKET, SO_REUSEADDR, reinterpret_cast<char const*>(&reuse), sizeof(reuse)) < 0) {
				throw std::runtime_error("TcpSocketFactory::CreateServer.setsockopt(SO_REUSEADDR)");
			}
#ifdef SO_REUSEPORT
			if (setsockopt(server, SOL_SOCKET, SO_REUSEPORT, &reuse, sizeof(reuse)) < 0) {
				throw std::runtime_error("TcpSocketFactory::CreateServer.setsockopt(SO_REUSEPORT)");
			}
#endif
			addressSize = static_cast<int>(rp->ai_addrlen);
			if (bind(server, rp->ai_addr, addressSize) == 0) {
				break;
			}
#ifdef _DEBUG
			std::cerr << "bind failed: " << errno << std::endl;
#endif
			close(server);
			server = INVALID_SOCKET;
		}
		freeaddrinfo(addresses);
	} catch (std::exception const&) {
		freeaddrinfo(addresses);
		throw;
	}
	if (server == INVALID_SOCKET) {
		throw std::runtime_error("TcpSocketFactory::CreateServer.socket");
	}

	// CreateServer for client connections.
	check(listen(server, 1));
	Accept(server, addressSize, onConnect);
}

void TcpSocketFactory::Accept(SOCKET server, socklen_t addressSize, fn_t onConnect) {
	try {
		// Await connections and requests.
		SOCKADDR_STORAGE address;
		auto* p = reinterpret_cast<sockaddr*>(&address);
		for (;;) {
			SOCKET socket = accept(server, p, &addressSize);
			if (socket == INVALID_SOCKET) {
				// Assume the failure is due to the network infrastructure
				// rejecting the connection.
				std::cerr << "failed connection: " << errno << std::endl;
				continue;
			}
#ifdef _DEBUG
			char node[NI_MAXHOST], service[NI_MAXSERV];
			if (getnameinfo(p, addressSize, node, sizeof(node), service, sizeof(service), NI_NUMERICHOST | NI_NUMERICSERV) == 0) {
				std::cout << "new connection: " << socket << '@' << node << ':' << service << std::endl;
			} else {
				std::cerr << "cannot get name information for accepted socket of family " << address.ss_family << std::endl;
			}
#endif

			// This is a new socket connection.  Handle it.
			onConnect(TcpSocket(socket));
		}
	} catch (std::exception& ex) {
		std::cerr << "exception: " << ex.what() << std::endl;
	}

	// Close the server socket and exit.
	close(server);
}
