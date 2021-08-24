#include "pch.h"
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

	SOCKET WSAAPI test_accept(SOCKET s, sockaddr* addr, socklen_t* addrlen) {
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

	int WSAAPI test_WSAPoll(pollfd* fdArray, std::uint32_t fds, int timeout) {
		return OnPoll(fdArray, fds, timeout);
	}

	int WSAAPI test_ioctlsocket(SOCKET s, long cmd, u_long* argp) {
		return OnIoctl(s, cmd, argp);
	}
}

#ifdef _WIN32
extern "C" intptr_t __imp_accept;
extern "C" intptr_t __imp_recv;
extern "C" intptr_t __imp_send;
extern "C" intptr_t __imp_closesocket;
extern "C" intptr_t __imp_WSAPoll;
extern "C" intptr_t __imp_ioctlsocket;
#endif

namespace Http_Test {
	namespace Sockets {
		std::function<SOCKET(SOCKET, sockaddr* addr, socklen_t* addrlen)> OnAccept;
		std::function<int(SOCKET, char* buf, int len, int flags)> OnReceive;
		std::function<int(SOCKET, char const* buf, int len, int flags)> OnSend;
		std::function<int(SOCKET)> OnClose;
		std::function<int(pollfd* fdArray, std::uint32_t fds, int timeout)> OnPoll;
		std::function<int(SOCKET s, long cmd, u_long* argp)> OnIoctl;
	}

	TEST_MODULE_INITIALIZE(ConfigureSockets) {
#ifdef _WIN32
		original_accept = Replace(test_accept, __imp_accept);
		original_recv = Replace(test_recv, __imp_recv);
		original_send = Replace(test_send, __imp_send);
		original_closesocket = Replace(test_closesocket, __imp_closesocket);
		original_WSAPoll = Replace(test_WSAPoll, __imp_WSAPoll);
		original_ioctlsocket = Replace(test_ioctlsocket, __imp_ioctlsocket);
#endif
	}
}

void Http_Test::Sockets::Initialize() {
	OnAccept = [](SOCKET s, sockaddr* addr, socklen_t* addrlen) { return INVALID_SOCKET; };
	OnReceive = [](SOCKET, char* buf, int len, int flags) { return 0; };
	OnSend = [](SOCKET, char const* buf, int len, int flags) { return 0; };
	OnClose = [](SOCKET s) { reinterpret_cast<decltype(closesocket)*>(original_closesocket)(s); return 0; };
	OnPoll = [](pollfd* fdArray, std::uint32_t fds, int timeout) { return 0; };
	OnIoctl = [](SOCKET s, long cmd, u_long* argp) { return 0; };
}

#ifndef _WIN32
int accept(int sockfd, struct sockaddr* addr, socklen_t* addrlen) {
	return test_accept(sockfd, addr, addrlen);
}

ssize_t recv(int sockfd, void* buf, size_t len, int flags) {
	return test_recv(sockfd, reinterpret_cast<char*>(buf), len, flags);
}

ssize_t send(int sockfd, void const* buf, size_t len, int flags) {
	return test_send(sockfd, reinterpret_cast<char const*>(buf), len, flags);
}

int close(int fd) {
	return test_closesocket(fd);
}

int poll(pollfd* fds, nfds_t nfds, int timeout) {
	return test_WSAPoll(fds, nfds, timeout);
}

int ioctl(int fd, unsigned long request, ...) {
	va_list argp;
	va_start(argp, request);
	auto* p = va_arg(argp, u_long*);
	return test_ioctlsocket(fd, request, p);
}

int fcntl(int fd, int cmd, ...) {
	return fd & cmd & (1 << 31);
}
#endif
