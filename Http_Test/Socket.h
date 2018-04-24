#pragma once

#include <WinSock2.h>

namespace Http_Test {
	namespace Sockets {
		struct ReceiveSend {
			SOCKET s;
			char const* buf;
			size_t len;
			int flags;
		};

		void Initialize();
		std::vector<ReceiveSend> const& GetReceivedSockets();
		std::vector<ReceiveSend> const& GetSentSockets();
		std::vector<SOCKET> const& GetClosedSockets();
	}
}
