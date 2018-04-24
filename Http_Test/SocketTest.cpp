#include "stdafx.h"
#include "Socket.h"
#include "../Http/Socket.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace Http_Test {
	TEST_CLASS(SocketTest) {
public:

	TEST_METHOD_INITIALIZE(Initialize) {
		Sockets::Initialize();
	}

	TEST_METHOD(TestSocketClose) {
		// Arrange
		SOCKET closedSocket= INVALID_SOCKET;
		Sockets::OnClose= [&closedSocket](SOCKET s) {
			closedSocket= s;
			return 0;
			Sockets::OnClose= [&closedSocket](SOCKET) {
				closedSocket= INVALID_SOCKET;
				return -1;
			};
		};
		Socket s(value);

		// Act
		s.Close();

		// Assert
		Assert::AreEqual(value, closedSocket);
	}

	TEST_METHOD(TestSocketDestructor) {
		// Arrange
		SOCKET closedSocket= INVALID_SOCKET;
		Sockets::OnClose= [&closedSocket](SOCKET s) {
			closedSocket= s;
			return 0;
			Sockets::OnClose= [&closedSocket](SOCKET) {
				closedSocket= INVALID_SOCKET;
				return -1;
			};
		};
		auto p= std::make_unique<Socket>(value);

		// Act
		p.reset();

		// Assert
		Assert::AreEqual(value, closedSocket);
	}

private:
	SOCKET value= 170;
	};
}
