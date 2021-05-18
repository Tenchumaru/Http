#include "pch.h"
#include "Sockets.h"
#include "../Http/TcpSocket.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace Http_Test {
	TEST_CLASS(TcpSocketTest) {
public:
	TEST_METHOD_INITIALIZE(Initialize) {
		Sockets::Initialize();
	}

	TEST_METHOD(TcpSocketReceive) {
		// Arrange
		SOCKET actualSocket = INVALID_SOCKET;
		char* actualBuf = nullptr;
		int actualLen = 0;
		int actualFlags = 0;
		Sockets::OnReceive = [&](SOCKET s, char* buf, int len, int flags) {
			actualSocket = s;
			actualBuf = buf;
			actualLen = len;
			actualFlags = flags;
			return 0;
		};
		TcpSocket s(expectedSocket);
		char buf[217];

		// Act
		auto result = s.Receive(buf, sizeof(buf));

		// Assert
		Assert::AreEqual(0, result.second);
		Assert::AreEqual(buf, actualBuf);
		Assert::AreEqual(sizeof(buf), static_cast<size_t>(actualLen));
		Assert::AreEqual(0, actualFlags);
		Assert::AreEqual(expectedSocket, actualSocket);
	}

	TEST_METHOD(TcpSocketSend) {
		// Arrange
		SOCKET actualSocket = INVALID_SOCKET;
		char const* actualBuf = nullptr;
		int actualLen = 0;
		int actualFlags = 0;
		Sockets::OnSend = [&](SOCKET s, char const* buf, int len, int flags) {
			actualSocket = s;
			actualBuf = buf;
			actualLen = len;
			actualFlags = flags;
			return 0;
		};
		TcpSocket s(expectedSocket);
		char buf[139];

		// Act
		auto result = s.Send(buf, sizeof(buf));

		// Assert
		Assert::AreEqual(0, result.second);
		Assert::AreEqual(buf, actualBuf);
		Assert::AreEqual(sizeof(buf), static_cast<size_t>(actualLen));
		Assert::AreEqual(0, actualFlags);
		Assert::AreEqual(expectedSocket, actualSocket);
	}

private:
	SOCKET expectedSocket = 230;
	};
}
