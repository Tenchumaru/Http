#include "pch.h"
#include "Sockets.h"
#include "../Http/Http.h"
#include "../Http/StaticHttpServer.h"
#include "Dispatch.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using Http_Test::Dispatch::ptr_t;

namespace Http_Test {
	TEST_CLASS(HttpServerTest) {
public:
	TEST_METHOD_INITIALIZE(Initialize) {
		Sockets::Initialize();
	}

	TEST_METHOD(StaticHttpServerRun) {
		// Arrange
		static char const request[] = "GET /f/15 HTTP/1.1\r\n"
			"Host: localhost:6006\r\n"
			"User-Agent: Mozilla/5.0 (Windows NT 10.0; Win64; x64; rv:59.0) Gecko/20100101 Firefox/59.0\r\n"
			"Accept: text/html,application/xhtml+xml,application/xml;q=0.9,*/*;q=0.8\r\n"
			"Accept-Language: en-US,en;q=0.5\r\n"
			"Accept-Encoding: gzip, deflate\r\n"
			"DNT: 1\r\n"
			"Connection: keep-alive\r\n"
			"Upgrade-Insecure-Requests: 1\r\n"
			"\r\n";
		std::optional<bool> pollValuesValid;
		Sockets::OnPoll = [&](pollfd* fdArray, std::uint32_t fds, int timeout) {
			if (fds == 1) {
				if (!pollValuesValid) {
					pollValuesValid = true;
				}
				fdArray->revents = fdArray->events & (POLLIN | POLLOUT);
				return 0;
			}
			pollValuesValid = false;
			return -1;
		};
		SOCKET expectedSocket = INVALID_SOCKET;
		auto* mainFiber = ConvertThreadToFiber(nullptr);
		int acceptInvocationCount = 0;
		bool acceptValuesValid = true;
		Sockets::OnAccept = [&](SOCKET s, sockaddr* addr, socklen_t* addrlen) {
			if (addr || addrlen) {
				WSASetLastError(10001);
				acceptValuesValid = false;
				return INVALID_SOCKET;
			}
			switch (++acceptInvocationCount) {
			case 1:
				WSASetLastError(WSAEWOULDBLOCK);
				return INVALID_SOCKET;
			case 2:
				expectedSocket = s | 0x10000;
				return expectedSocket;
			default:
				SwitchToFiber(mainFiber);
			}
			throw std::logic_error("unexpected acceptInvocationCount");
		};
		std::optional<bool> receiveValuesValid;
		Sockets::OnReceive = [&](SOCKET s, char* buf, int len, int flags) {
			if (expectedSocket != s || flags != 0) {
				WSASetLastError(10010);
				receiveValuesValid = false;
				return -1;
			}
			if (receiveValuesValid.has_value()) {
				return 0;
			}
			constexpr int size = _countof(request) - 1;
			if (len < size) {
				receiveValuesValid = false;
				return -1;
			}
			receiveValuesValid = true;
			memcpy_s(buf, len, request, size);
			return size;
		};
		std::string actualRequest;
		Dispatch::OnDispatch = [&actualRequest](ptr_t begin, ptr_t body, ptr_t end, ClientSocket& socket, Response& response) {
			actualRequest.assign(begin, body);
			return end;
		};
		StaticHttpServer server;

		// Act
		std::string actualMessage;
		server.Run(6006);

		// Assert
		Assert::AreEqual(3, acceptInvocationCount);
		Assert::IsTrue(acceptValuesValid);
		Assert::IsTrue(pollValuesValid.has_value() && pollValuesValid.value());
		Assert::IsTrue(receiveValuesValid.has_value() && receiveValuesValid.value());
		Assert::AreEqual(std::string(request), actualRequest);
	}
	};
}
