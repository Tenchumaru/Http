#pragma once

#include <WS2tcpip.h>

namespace Http_Test {
	namespace Sockets {
		struct Accept {
			SOCKET s;
			struct sockaddr* addr;
			int* addrlen;
		};

		struct ReceiveSend {
			SOCKET s;
			char const* buf;
			size_t len;
			int flags;
		};

		void Initialize();
		std::vector<Accept> const& GetAcceptedSockets();
		std::vector<ReceiveSend> const& GetReceivedSockets();
		std::vector<ReceiveSend> const& GetSentSockets();
		std::vector<SOCKET> const& GetClosedSockets();
	}
}
