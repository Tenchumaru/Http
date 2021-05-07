#include "pch.h"
#include "TcpSocket.h"
#include "SocketServer.h"

using namespace std::literals;

std::pair<SOCKET, socklen_t> SocketServer::Open(char const* service) {
#ifdef _WIN32
	// Initialize the socket library.
	static bool isInitialized = [] {
		WSADATA wsaData;
		auto const result = WSAStartup(MAKEWORD(2, 2), &wsaData);
		if (result) {
			std::cerr << "WSAStartup error " << result << std::endl;
			throw std::runtime_error("SocketServer::Open.WSAStartup");
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
		throw std::runtime_error("SocketServer::Open.getaddrinfo");
	}
	SOCKET serverSocket = INVALID_SOCKET;
	socklen_t addressSize = 0;
	for (auto const* address = addresses; address != nullptr; address = address->ai_next) {
		serverSocket = socket(address->ai_family, address->ai_socktype, address->ai_protocol);
		if (serverSocket == INVALID_SOCKET) {
			continue;
		}
		if (TrySocketUse(serverSocket, address, addressSize)) {
			break;
		}
		close(serverSocket);
		serverSocket = INVALID_SOCKET;
	}
	freeaddrinfo(addresses);
	if (serverSocket == INVALID_SOCKET) {
		throw std::runtime_error("SocketServer::Open");
	}

	// Listen for client connections.
	check(listen(serverSocket, 1));
	return { serverSocket, addressSize };
}

void SocketServer::Run(char const* service) {
	try {
		// Create the server socket.
		auto [serverSocket, addressSize] = Open(service);

		// Accept and handle connections from it.
		for (;;) {
			auto [clientSocket, errorCode] = Accept(serverSocket, addressSize);
			if (!errorCode) {
				Handle(std::move(clientSocket));
			}
		}
	} catch (std::exception const& ex) {
		std::cerr << "SocketServer::Run failed:  " << ex.what() << std::endl;
	}
}

void SocketServer::Run(unsigned short port) {
	char buf[6];
	sprintf_s(buf, "%d", port);
	Run(buf);
}

SOCKET SocketServer::Connect(char const* nodeName, char const* serviceName) {
	// Create and connect a client socket.
	addrinfo hints{};
	hints.ai_family = AF_UNSPEC; // Allow IPv4 or IPv6.
	hints.ai_socktype = SOCK_STREAM;
	addrinfo* addresses;
	if (getaddrinfo(nodeName, serviceName, &hints, &addresses)) {
		throw std::runtime_error("SocketServer::Connect.getaddrinfo");
	}
	SOCKET clientSocket = INVALID_SOCKET;
	for (auto const* address = addresses; address != nullptr; address = address->ai_next) {
		clientSocket = socket(address->ai_family, address->ai_socktype, address->ai_protocol);
		if (clientSocket == INVALID_SOCKET) {
			continue;
		}
		if (ConnectImpl(clientSocket, address->ai_addr, address->ai_addrlen) == 0) {
			break;
		}
		close(clientSocket);
		clientSocket = INVALID_SOCKET;
	}
	freeaddrinfo(addresses);
	if (clientSocket == INVALID_SOCKET) {
		throw std::runtime_error("SocketServer::Connect");
	}
	return clientSocket;
}

SOCKET SocketServer::Connect(char const* host, std::uint16_t port) {
	char s[6];
	sprintf_s(s, "%d", port);
	return Connect(host, s);
}

std::pair<SOCKET, int> SocketServer::Accept(SOCKET serverSocket, socklen_t addressSize) {
	auto n = addressSize;
	SOCKADDR_INET address;
	auto const clientSocket = accept(serverSocket, reinterpret_cast<sockaddr*>(&address), &n);
	if (clientSocket == INVALID_SOCKET) {
		std::cerr << "SocketServer::Accept.accept error " << errno << std::endl;
	}
#ifdef _DEBUG
	else {
		std::cout << "accepted socket " << clientSocket << ", address family " << address.si_family << std::endl;
		if (address.si_family == AF_INET || address.si_family == AF_INET6) {
			char s[std::max(INET_ADDRSTRLEN, INET6_ADDRSTRLEN)];
			auto* const p = inet_ntop(address.si_family, &address, s, sizeof(s));
			if (p) {
				std::cout << "\tIP address " << p << ", port " << address.Ipv4.sin_port << std::endl;
			} else {
				std::cerr << "\tcannot determine address; error " << errno << std::endl;
			}
		} else {
			std::cout << "\tunknown address family" << std::endl;
		}
	}
#endif
	return{ clientSocket, clientSocket == INVALID_SOCKET ? errno : 0 };
}

int SocketServer::ConnectImpl(SOCKET clientSocket, sockaddr const* address, size_t addressSize) noexcept {
	return connect(clientSocket, address, static_cast<socklen_t>(addressSize)) ? errno : 0;
}

bool SocketServer::TrySocketUse(SOCKET serverSocket, addrinfo const* address, socklen_t& addressSize) {
	int const reuse = 1;
	if (setsockopt(serverSocket, SOL_SOCKET, SO_REUSEADDR, reinterpret_cast<char const*>(&reuse), sizeof(reuse))) {
		std::cerr << "SocketServer::TrySocketUse.setsockopt(SO_REUSEADDR) error " << errno << std::endl;
	}
#ifdef SO_REUSEPORT
	else if (setsockopt(serverSocket, SOL_SOCKET, SO_REUSEPORT, &reuse, sizeof(reuse))) {
		std::cerr << "SocketServer::TrySocketUse.setsockopt(SO_REUSEPORT) error " << errno << std::endl;
	}
#endif
	else {
		addressSize = static_cast<std::remove_reference_t<decltype(addressSize)>>(address->ai_addrlen);
		if (bind(serverSocket, address->ai_addr, addressSize) == 0) {
			return true;
		}
	}
#ifdef _DEBUG
	std::cerr << "SocketServer::TrySocketUse.bind error " << errno << std::endl;
#endif
	return false;
}
