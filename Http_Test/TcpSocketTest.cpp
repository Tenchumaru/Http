#include "stdafx.h"
#include "Socket.h"
#include "../Http/TcpSocket.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace Http_Test {
	TEST_CLASS(TcpSocketTest) {
public:

	TEST_METHOD_INITIALIZE(Initialize) {
		Sockets::Initialize();
	}

	TEST_METHOD(TestTcpSocketReceive) {
		// Arrange
		TcpSocket s(value);
		char buf[217];

		// Act
		int result= s.Receive(buf, sizeof(buf));

		// Assert
		Assert::AreEqual(0, result);
		auto const& receivedSockets= Sockets::GetReceivedSockets();
		Assert::AreEqual(1ull, receivedSockets.size());
		auto const& receivedSocket= receivedSockets.back();
		Assert::AreEqual(buf, receivedSocket.buf);
		Assert::AreEqual(0, receivedSocket.flags);
		Assert::AreEqual(sizeof(buf), receivedSocket.len);
		Assert::AreEqual(value, receivedSocket.s);
	}

	TEST_METHOD(TestTcpSocketSend) {
		// Arrange
		TcpSocket s(value);
		char buf[139];

		// Act
		int result= s.Send(buf, sizeof(buf));

		// Assert
		Assert::AreEqual(0, result);
		auto const& sentSockets= Sockets::GetSentSockets();
		Assert::AreEqual(1ull, sentSockets.size());
		auto const& sentSocket= sentSockets.back();
		Assert::AreEqual(buf, sentSocket.buf);
		Assert::AreEqual(0, sentSocket.flags);
		Assert::AreEqual(sizeof(buf), sentSocket.len);
		Assert::AreEqual(value, sentSocket.s);
	}

private:
	SOCKET value= 230;
	};
}
