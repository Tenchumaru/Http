#include "stdafx.h"
#include "Socket.h"
#include "../Http/TcpSocketFactory.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace Http_Test {
	TEST_CLASS(TcpSocketTest) {
public:

	TEST_METHOD_INITIALIZE(Initialize) {
		Sockets::Initialize();
	}

	TEST_METHOD(CreateServerWithPort) {
		// Arrange
		SOCKET actualSocket= INVALID_SOCKET;
		Sockets::OnAccept= [&](SOCKET s, struct sockaddr* addr, int* addrlen) {
			actualSocket= s;
			addr, addrlen;
			return s | 0x10000;
		};
		std::vector<SOCKET> closedSockets;
		Sockets::OnClose= [&closedSockets](SOCKET s) {
			closedSockets.push_back(s);
			return 0;
		};
		bool invoked= false;
		auto fn= [&invoked](TcpSocket&&) {
			invoked= true;
			throw std::runtime_error("exit");
		};
		TcpSocketFactory factory;

		// Act
		factory.CreateServer(6006, fn);

		// Assert
		Assert::IsTrue(invoked);
		Assert::AreEqual(2ull, closedSockets.size());
		Assert::AreEqual(closedSockets.front(), actualSocket | 0x10000);
		Assert::AreEqual(closedSockets.back(), actualSocket);
	}
	};
}
