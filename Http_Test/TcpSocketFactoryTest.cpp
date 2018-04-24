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
		auto const& acceptedSockets= Sockets::GetAcceptedSockets();
		Assert::AreEqual(1ull, acceptedSockets.size());
		auto const& closedSockets= Http_Test::Sockets::GetClosedSockets();
		Assert::AreEqual(2ull, closedSockets.size());
	}
	};
}
