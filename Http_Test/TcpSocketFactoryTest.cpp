#include "stdafx.h"
#include "Sockets.h"
#include "../Http/TcpSocketFactory.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace Http_Test {
	TEST_CLASS(TcpSocketFactoryTest) {
public:

	TEST_METHOD_INITIALIZE(Initialize) {
		Sockets::Initialize();
	}

	TEST_METHOD(TcpSocketFactoryCreateServerWithName) {
		// Arrange
		SOCKET expectedSocket= INVALID_SOCKET;
		Sockets::OnAccept= [&](SOCKET s, sockaddr* addr, int* addrlen) {
			if(addr == nullptr || addrlen == nullptr) {
				return INVALID_SOCKET;
			}
			expectedSocket= s;
			return s | 0x10000;
		};
		std::vector<SOCKET> closedSockets;
		Sockets::OnClose= [&closedSockets](SOCKET s) {
			closedSockets.push_back(s);
			return 0;
		};
		bool invoked= false;
		auto onConnect= [&invoked](TcpSocket&&) {
			invoked= true;
			throw std::runtime_error("exit");
		};
		TcpSocketFactory factory;

		// Act
		factory.CreateServer("http", onConnect);

		// Assert
		Assert::IsTrue(invoked);
		Assert::AreNotEqual(expectedSocket, INVALID_SOCKET);
		Assert::AreEqual(2ull, closedSockets.size());
		if (!closedSockets.empty()) {
			Assert::AreEqual(expectedSocket | 0x10000, closedSockets.front());
			Assert::AreEqual(expectedSocket, closedSockets.back());
		}
	}

	TEST_METHOD(TcpSocketFactoryCreateServerWithPort) {
		// Arrange
		SOCKET expectedSocket= INVALID_SOCKET;
		Sockets::OnAccept= [&](SOCKET s, sockaddr* addr, int* addrlen) {
			if(addr == nullptr || addrlen == nullptr) {
				return INVALID_SOCKET;
			}
			expectedSocket= s;
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
		Assert::AreNotEqual(expectedSocket, INVALID_SOCKET);
		Assert::AreEqual(2ull, closedSockets.size());
		if(!closedSockets.empty()) {
			Assert::AreEqual(expectedSocket | 0x10000, closedSockets.front());
			Assert::AreEqual(expectedSocket, closedSockets.back());
		}
	}
	};
}
