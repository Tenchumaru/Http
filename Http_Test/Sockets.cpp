#include "stdafx.h"
#include "Sockets.h"

using namespace Http_Test::Sockets;

namespace {
	template<typename F>
	intptr_t Replace(F f, intptr_t& imp) {
		auto p = reinterpret_cast<intptr_t>(f);
		std::swap(p, imp);
		return p;
	}

	intptr_t original_accept;
	intptr_t original_getnameinfo;
	intptr_t original_recv;
	intptr_t original_send;
	intptr_t original_closesocket;
	intptr_t original_WSAPoll;
	intptr_t original_ioctlsocket;

	SOCKET WSAAPI test_accept(SOCKET s, struct sockaddr* addr, int* addrlen) {
		return OnAccept(s, addr, addrlen);
	}

	INT WSAAPI test_getnameinfo(const SOCKADDR*, socklen_t, PCHAR pNodeBuffer, DWORD NodeBufferSize, PCHAR pServiceBuffer, DWORD ServiceBufferSize, INT) {
		strncpy_s(pNodeBuffer, NodeBufferSize, "test", NodeBufferSize);
		strncpy_s(pServiceBuffer, ServiceBufferSize, "test", ServiceBufferSize);
		return 0;
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
extern "C" intptr_t __imp_getnameinfo;
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
		original_accept = Replace(test_accept, __imp_accept);
		original_getnameinfo = Replace(test_getnameinfo, __imp_getnameinfo);
		original_recv = Replace(test_recv, __imp_recv);
		original_send = Replace(test_send, __imp_send);
		original_closesocket = Replace(test_closesocket, __imp_closesocket);
		original_WSAPoll = Replace(test_WSAPoll, __imp_WSAPoll);
		original_ioctlsocket = Replace(test_ioctlsocket, __imp_ioctlsocket);
	}
}

void Http_Test::Sockets::Initialize() {
	OnAccept = [](SOCKET s, struct sockaddr* addr, int* addrlen) { return INVALID_SOCKET; };
	OnReceive = [](SOCKET, char* buf, int len, int flags) { return 0; };
	OnSend = [](SOCKET, char const* buf, int len, int flags) { return 0; };
	OnClose = [](SOCKET s) { reinterpret_cast<decltype(closesocket)*>(original_closesocket)(s); return 0; };
	OnPoll = [](LPWSAPOLLFD fdArray, ULONG fds, INT timeout) { return 0; };
	OnIoctl = [](SOCKET s, long cmd, u_long* argp) { return 0; };
}
