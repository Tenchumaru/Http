#include "stdafx.h"
#include "Socket.h"

using namespace Http_Test::Sockets;

namespace {
	SOCKET WSAAPI test_accept(SOCKET s, struct sockaddr* addr, int* addrlen) {
		return OnAccept(s, addr, addrlen);
	}

	int WSAAPI test_recv(SOCKET s, char* buf, int len, int flags) {
		return OnReceive(s, buf, len, flags);
	}

	int WSAAPI test_send(SOCKET s, char const* buf, int len, int flags) {
		return OnSend(s, buf, len, flags);
	}

	int WSAAPI test_closesocket(SOCKET s) {
		return OnClose(s);
	}
}

extern "C" intptr_t __imp_accept;
extern "C" intptr_t __imp_recv;
extern "C" intptr_t __imp_send;
extern "C" intptr_t __imp_closesocket;

namespace Http_Test {
	namespace Sockets {
		std::function<SOCKET(SOCKET, struct sockaddr* addr, int* addrlen)> OnAccept;
		std::function<int(SOCKET, char* buf, int len, int flags)> OnReceive;
		std::function<int(SOCKET, char const* buf, int len, int flags)> OnSend;
		std::function<int(SOCKET)> OnClose;
	}

	TEST_MODULE_INITIALIZE(ConfigureSockets) {
		__imp_accept= reinterpret_cast<decltype(__imp_accept)>(&test_accept);
		__imp_recv= reinterpret_cast<decltype(__imp_recv)>(&test_recv);
		__imp_send= reinterpret_cast<decltype(__imp_send)>(&test_send);
		__imp_closesocket= reinterpret_cast<decltype(__imp_closesocket)>(&test_closesocket);
	}
}

void Http_Test::Sockets::Initialize() {
	OnAccept= [](SOCKET s, struct sockaddr* addr, int* addrlen) { return INVALID_SOCKET; };
	OnReceive= [](SOCKET, char* buf, int len, int flags) { return 0; };
	OnSend= [](SOCKET, char const* buf, int len, int flags) { return 0; };
	OnClose= [](SOCKET) { return 0; };
}
