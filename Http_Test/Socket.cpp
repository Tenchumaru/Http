#include "stdafx.h"
#include "Socket.h"

using namespace Http_Test::Sockets;

namespace {
	std::vector<Accept> acceptedSockets;

	SOCKET WSAAPI test_accept(SOCKET s, struct sockaddr* addr, int* addrlen) {
		if(addr == nullptr || addrlen == nullptr) {
			return INVALID_SOCKET;
		}
		acceptedSockets.push_back({ s, addr, addrlen });
		return s | (0x10000 * acceptedSockets.size());
	}

	std::vector<ReceiveSend> receivedSockets;

	int WSAAPI test_recv(SOCKET s, char* buf, int len, int flags) {
		if(buf == nullptr) {
			return -1;
		}
		receivedSockets.push_back({ s, buf, static_cast<size_t>(len), flags });
		return 0;
	}

	std::vector<ReceiveSend> sentSockets;

	int WSAAPI test_send(SOCKET s, char const* buf, int len, int flags) {
		if(buf == nullptr) {
			return -1;
		}
		sentSockets.push_back({ s, buf, static_cast<size_t>(len), flags });
		return 0;
	}

	std::vector<SOCKET> closedSockets;

	int WSAAPI test_closesocket(SOCKET s) {
		closedSockets.push_back(s);
		return 0;
	}
}

extern "C" intptr_t __imp_accept;
extern "C" intptr_t __imp_recv;
extern "C" intptr_t __imp_send;
extern "C" intptr_t __imp_closesocket;

namespace Http_Test {
	TEST_MODULE_INITIALIZE(ConfigureSockets) {
		__imp_accept= reinterpret_cast<decltype(__imp_accept)>(&test_accept);
		__imp_recv= reinterpret_cast<decltype(__imp_recv)>(&test_recv);
		__imp_send= reinterpret_cast<decltype(__imp_send)>(&test_send);
		__imp_closesocket= reinterpret_cast<decltype(__imp_closesocket)>(&test_closesocket);
	}
}

void Http_Test::Sockets::Initialize() {
	acceptedSockets.clear();
	receivedSockets.clear();
	sentSockets.clear();
	closedSockets.clear();
}

std::vector<Accept> const& Http_Test::Sockets::GetAcceptedSockets() {
	return acceptedSockets;
}

std::vector<ReceiveSend> const& Http_Test::Sockets::GetReceivedSockets() {
	return receivedSockets;
}

std::vector<ReceiveSend> const& Http_Test::Sockets::GetSentSockets() {
	return sentSockets;
}

std::vector<SOCKET> const& Http_Test::Sockets::GetClosedSockets() {
	return closedSockets;
}
