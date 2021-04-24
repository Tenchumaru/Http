#include "pch.h"
#include "Sockets.h"
#include "../Http/Waiter.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace Http_Test {
	TEST_CLASS(WaiterTest) {
public:

	TEST_METHOD_INITIALIZE(Initialize) {
		Sockets::Initialize();
	}

	TEST_METHOD(WaiterAdd) {
		// Arrange
		Waiter waiter;

		// Act
		waiter.Add(expectedSocket, POLLIN);

		// Assert
		auto* p = reinterpret_cast<std::vector<pollfd>*>(&waiter) + 2;
		auto* next = reinterpret_cast<std::vector<pollfd>**>(p)[1];
		Assert::AreEqual(1ull, next->size());
	}

	TEST_METHOD(WaiterWait) {
		// Arrange
		bool invoked = false;
		Sockets::OnPoll = [&](LPWSAPOLLFD fdArray, ULONG fds, INT timeout) {
			if (fdArray == nullptr || fds != 1) {
				return -1;
			}
			invoked = true;
			fdArray[0].fd = expectedSocket;
			fdArray[0].revents = POLLIN;
			return 0;
		};
		Waiter waiter;
		waiter.Add(expectedSocket, POLLIN);

		// Act
		auto actualSocket = waiter.Wait();

		// Assert
		Assert::IsTrue(invoked);
		Assert::AreEqual(expectedSocket, actualSocket);
	}

private:
	SOCKET expectedSocket = 410;
	};
}
