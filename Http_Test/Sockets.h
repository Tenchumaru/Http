#pragma once

namespace Http_Test {
	namespace Sockets {
		extern std::function<SOCKET(SOCKET, struct sockaddr* addr, socklen_t* addrlen)> OnAccept;
		extern std::function<SOCKET(SOCKET, struct sockaddr const* addr, socklen_t addrlen)> OnBind;
		extern std::function<SOCKET(SOCKET, int backlog)> OnListen;
		extern std::function<int(SOCKET, char* buf, int len, int flags)> OnReceive;
		extern std::function<int(SOCKET, char const* buf, int len, int flags)> OnSend;
		extern std::function<int(SOCKET)> OnClose;
		extern std::function<int(int domain, int type, int protocol)> OnSocket;
		extern std::function<int(int socket, int level, int option_name, void const* option_value, socklen_t option_len)> OnSetSockOpt;
		extern std::function<int(pollfd* fdArray, std::uint32_t fds, int timeout)> OnPoll;
		extern std::function<int(SOCKET s, long cmd, u_long* argp)> OnIoctl;

		void Initialize();
	}
}
