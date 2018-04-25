#include "stdafx.h"
#include "Socket.h"
#include "../Http/Waiter.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace Http_Test {
	TEST_CLASS(WaiterTest) {
public:

	TEST_METHOD_INITIALIZE(Initialize) {
		Sockets::Initialize();
	}

	TEST_METHOD(TestWaiterAdd) {
		// Arrange
		bool invoked= false;
		SOCKET actualSocket;
		long actualCmd;
		u_long actualArg;
		Sockets::OnIoctl= [&](SOCKET s, long cmd, u_long* argp) {
			if(s == INVALID_SOCKET || argp == nullptr) {
				return -1;
			}
			invoked= true;
			actualSocket= s;
			actualCmd= cmd;
			actualArg= *argp;
			return 0;
		};
		Waiter waiter;

		// Act
		waiter.Add(value, POLLIN);

		// Assert
		Assert::IsTrue(invoked);
		Assert::AreEqual(value, actualSocket);
		Assert::AreEqual(static_cast<decltype(actualCmd)>(FIONBIO), actualCmd);
		Assert::AreEqual(1ul, actualArg);
	}

	TEST_METHOD(TestWaiterWait) {
		// Arrange
		bool invoked= false;
		Sockets::OnPoll= [&](LPWSAPOLLFD fdArray, ULONG fds, INT timeout) {
			if(fdArray == nullptr || fds != 1) {
				return -1;
			}
			invoked= true;
			fdArray[0].fd= value;
			fdArray[0].revents= POLLIN;
			return 0;
		};
		Waiter waiter;
		waiter.Add(value, POLLIN);

		// Act
		auto actualSocket= waiter.Wait();

		// Assert
		Assert::IsTrue(invoked);
		Assert::AreEqual(value, actualSocket);
	}

	TEST_METHOD(TestWaiter) {
		// Arrange
		Sockets::OnIoctl= [](SOCKET s, long cmd, u_long* argp) {
			return 0;
		};
		Sockets::OnPoll= [](LPWSAPOLLFD fdArray, ULONG fds, INT timeout) {
			return 0;
		};
	}

private:
	SOCKET value= 410;
	};
}
