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
	intptr_t original_bind;
	intptr_t original_listen;
	intptr_t original_getnameinfo;
	intptr_t original_recv;
	intptr_t original_send;
	intptr_t original_closesocket;
	intptr_t original_socket;
	intptr_t original_setsockopt;
	intptr_t original_WSAPoll;
	intptr_t original_ioctlsocket;

	SOCKET WSAAPI test_accept(SOCKET s, sockaddr* addr, socklen_t* addrlen) {
		return OnAccept(s, addr, addrlen);
	}

	SOCKET WSAAPI test_bind(SOCKET s, sockaddr const* addr, socklen_t addrlen) {
		return OnBind(s, addr, addrlen);
	}

	SOCKET WSAAPI test_listen(SOCKET s, int backlog) {
		return OnListen(s, backlog);
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

	int WSAAPI test_socket(int domain, int type, int protocol) {
		return OnSocket(domain, type, protocol);
	}

	int WSAAPI test_setsockopt(int socket, int level, int option_name, void const* option_value, socklen_t option_len) {
		return OnSetSockOpt(socket, level, option_name, option_value, option_len);
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
extern "C" intptr_t __imp_bind;
extern "C" intptr_t __imp_listen;
extern "C" intptr_t __imp_recv;
extern "C" intptr_t __imp_send;
extern "C" intptr_t __imp_closesocket;
extern "C" intptr_t __imp_socket;
extern "C" intptr_t __imp_setsockopt;
extern "C" intptr_t __imp_WSAPoll;
extern "C" intptr_t __imp_ioctlsocket;
#endif

namespace Http_Test {
	namespace Sockets {
		std::function<SOCKET(SOCKET, sockaddr* addr, socklen_t* addrlen)> OnAccept;
		std::function<SOCKET(SOCKET, sockaddr const* addr, socklen_t addrlen)> OnBind;
		std::function<SOCKET(SOCKET, int backlog)> OnListen;
		std::function<int(SOCKET, char* buf, int len, int flags)> OnReceive;
		std::function<int(SOCKET, char const* buf, int len, int flags)> OnSend;
		std::function<int(SOCKET)> OnClose;
		std::function<int(int domain, int type, int protocol)> OnSocket;
		std::function<int(int socket, int level, int option_name, void const* option_value, socklen_t option_len)> OnSetSockOpt;
		std::function<int(pollfd* fdArray, std::uint32_t fds, int timeout)> OnPoll;
		std::function<int(SOCKET s, long cmd, u_long* argp)> OnIoctl;
	}

	TEST_MODULE_INITIALIZE(ConfigureSockets) {
#ifdef _WIN32
		original_accept = Replace(test_accept, __imp_accept);
		original_bind = Replace(test_bind, __imp_bind);
		original_listen = Replace(test_listen, __imp_listen);
		original_recv = Replace(test_recv, __imp_recv);
		original_send = Replace(test_send, __imp_send);
		original_closesocket = Replace(test_closesocket, __imp_closesocket);
		original_socket = Replace(test_socket, __imp_socket);
		original_setsockopt = Replace(test_setsockopt, __imp_setsockopt);
		original_WSAPoll = Replace(test_WSAPoll, __imp_WSAPoll);
		original_ioctlsocket = Replace(test_ioctlsocket, __imp_ioctlsocket);
#endif
	}
}

void Http_Test::Sockets::Initialize() {
	OnAccept = [](SOCKET s, sockaddr* addr, socklen_t* addrlen) { return INVALID_SOCKET; };
	OnBind = [](SOCKET s, sockaddr const* addr, socklen_t addrlen) { return 0; };
	OnListen = [](SOCKET s, int backlog) { return 0; };
	OnReceive = [](SOCKET, char* buf, int len, int flags) { return 0; };
	OnSend = [](SOCKET, char const* buf, int len, int flags) { return 0; };
	OnClose = [](SOCKET s) { return 0; };
	OnSocket = [](int domain, int type, int protocol) { return 9; };
	OnSetSockOpt = [](int socket, int level, int option_name, void const* option_value, socklen_t option_len) { return 0; };
	OnPoll = [](pollfd* fdArray, std::uint32_t fds, int timeout) { return 0; };
	OnIoctl = [](SOCKET s, long cmd, u_long* argp) { return 0; };
}

#ifndef _WIN32
int accept(int sockfd, struct sockaddr* addr, socklen_t* addrlen) {
	return test_accept(sockfd, addr, addrlen);
}

int bind(int sockfd, struct sockaddr const* addr, socklen_t addrlen) {
	return test_bind(sockfd, addr, addrlen);
}

int listen(int sockfd, int backlog) {
	return test_listen(sockfd, backlog);
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

int socket(int domain, int type, int protocol) {
	return test_socket(domain, type, protocol);
}

int setsockopt(int socket, int level, int option_name, void const* option_value, socklen_t option_len) {
	return test_setsockopt(socket, level, option_name, option_value, option_len);
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
