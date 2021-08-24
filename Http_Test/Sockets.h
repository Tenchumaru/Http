#pragma once

namespace Http_Test {
	namespace Sockets {
		extern std::function<SOCKET(SOCKET, struct sockaddr* addr, socklen_t* addrlen)> OnAccept;
		extern std::function<int(SOCKET, char* buf, int len, int flags)> OnReceive;
		extern std::function<int(SOCKET, char const* buf, int len, int flags)> OnSend;
		extern std::function<int(SOCKET)> OnClose;
		extern std::function<int(pollfd* fdArray, std::uint32_t fds, int timeout)> OnPoll;
		extern std::function<int(SOCKET s, long cmd, u_long* argp)> OnIoctl;

		void Initialize();
	}
}
