#include "stdafx.h"
#include "Sockets.h"

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

	int WSAAPI test_WSAPoll(LPWSAPOLLFD fdArray, ULONG fds, INT timeout) {
		return OnPoll(fdArray, fds, timeout);
	}

	int WSAAPI test_ioctlsocket(SOCKET s, long cmd, u_long* argp) {
		return OnIoctl(s, cmd, argp);
	}
}

extern "C" intptr_t __imp_accept;
extern "C" intptr_t __imp_recv;
extern "C" intptr_t __imp_send;
extern "C" intptr_t __imp_closesocket;
extern "C" intptr_t __imp_WSAPoll;
extern "C" intptr_t __imp_ioctlsocket;

namespace Http_Test {
	namespace Sockets {
		std::function<SOCKET(SOCKET, struct sockaddr* addr, int* addrlen)> OnAccept;
		std::function<int(SOCKET, char* buf, int len, int flags)> OnReceive;
		std::function<int(SOCKET, char const* buf, int len, int flags)> OnSend;
		std::function<int(SOCKET)> OnClose;
		std::function<int(LPWSAPOLLFD fdArray, ULONG fds, INT timeout)> OnPoll;
		std::function<int(SOCKET s, long cmd, u_long* argp)> OnIoctl;
	}

	TEST_MODULE_INITIALIZE(ConfigureSockets) {
		__imp_accept= reinterpret_cast<decltype(__imp_accept)>(&test_accept);
		__imp_recv= reinterpret_cast<decltype(__imp_recv)>(&test_recv);
		__imp_send= reinterpret_cast<decltype(__imp_send)>(&test_send);
		__imp_closesocket= reinterpret_cast<decltype(__imp_closesocket)>(&test_closesocket);
		__imp_WSAPoll= reinterpret_cast<decltype(__imp_WSAPoll)>(&test_WSAPoll);
		__imp_ioctlsocket= reinterpret_cast<decltype(__imp_ioctlsocket)>(&test_ioctlsocket);
	}
}

void Http_Test::Sockets::Initialize() {
	OnAccept= [](SOCKET s, struct sockaddr* addr, int* addrlen) { return INVALID_SOCKET; };
	OnReceive= [](SOCKET, char* buf, int len, int flags) { return 0; };
	OnSend= [](SOCKET, char const* buf, int len, int flags) { return 0; };
	OnClose= [](SOCKET) { return 0; };
	OnPoll= [](LPWSAPOLLFD fdArray, ULONG fds, INT timeout) { return 0; };
	OnIoctl= [](SOCKET s, long cmd, u_long* argp) { return 0; };
}
