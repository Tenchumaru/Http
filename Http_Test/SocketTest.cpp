#include "pch.h"
#include "Sockets.h"
#include "../Http/Socket.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace Http_Test {
	TEST_CLASS(SocketTest) {
public:

	TEST_METHOD_INITIALIZE(Initialize) {
		Sockets::Initialize();
	}

	TEST_METHOD(SocketClose) {
		// Arrange
		SOCKET closedSocket = INVALID_SOCKET;
		Sockets::OnClose = [&closedSocket](SOCKET s) {
			closedSocket = s;
			return 0;
			Sockets::OnClose = [&closedSocket](SOCKET) {
				closedSocket = INVALID_SOCKET;
				return -1;
			};
		};
		Socket s(expectedSocket);

		// Act
		s.Close();

		// Assert
		Assert::AreEqual(expectedSocket, closedSocket);
	}

	TEST_METHOD(SocketDestructor) {
		// Arrange
		SOCKET closedSocket = INVALID_SOCKET;
		Sockets::OnClose = [&closedSocket](SOCKET s) {
			closedSocket = s;
			return 0;
			Sockets::OnClose = [&closedSocket](SOCKET) {
				closedSocket = INVALID_SOCKET;
				return -1;
			};
		};
		auto p = std::make_unique<Socket>(expectedSocket);

		// Act
		p.reset();

		// Assert
		Assert::AreEqual(expectedSocket, closedSocket);
	}

private:
	SOCKET expectedSocket = 170;
	};
}
