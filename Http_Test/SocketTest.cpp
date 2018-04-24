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
		Socket s(value);

		// Act
		s.Close();

		// Assert
		auto const& closedSockets= Http_Test::Sockets::GetClosedSockets();
		Assert::AreEqual(1ull, closedSockets.size());
		Assert::AreEqual(value, closedSockets.back());
	}

	TEST_METHOD(TestSocketDestructor) {
		// Arrange
		auto p= std::make_unique<Socket>(value);

		// Act
		p.reset();

		// Assert
		auto const& closedSockets= Http_Test::Sockets::GetClosedSockets();
		Assert::AreEqual(1ull, closedSockets.size());
		Assert::AreEqual(value, closedSockets.back());
	}

private:
	SOCKET value= 170;
	};
}
