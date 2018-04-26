#pragma once

#include <WS2tcpip.h>

namespace Http_Test {
	namespace Sockets {
		extern std::function<SOCKET(SOCKET, struct sockaddr* addr, int* addrlen)> OnAccept;
		extern std::function<int(SOCKET, char* buf, int len, int flags)> OnReceive;
		extern std::function<int(SOCKET, char const* buf, int len, int flags)> OnSend;
		extern std::function<int(SOCKET)> OnClose;
		extern std::function<int(LPWSAPOLLFD fdArray, ULONG fds, INT timeout)> OnPoll;
		extern std::function<int(SOCKET s, long cmd, u_long* argp)> OnIoctl;

		void Initialize();
	}
}
