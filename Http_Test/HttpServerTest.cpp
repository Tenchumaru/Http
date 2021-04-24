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
		Sockets::OnPoll = [&](_Inout_ LPWSAPOLLFD fdArray, _In_ size_t fds, _In_ INT timeout) {
			if (fds == 1) {
				fdArray->revents = fdArray->events & (POLLIN | POLLOUT);
				return 0;
			}
			return -1;
		};
		SOCKET expectedSocket = INVALID_SOCKET;
		Sockets::OnAccept = [&](SOCKET s, sockaddr* addr, int* addrlen) {
			if (addr == nullptr || addrlen == nullptr) {
				return INVALID_SOCKET;
			}
			if (expectedSocket != INVALID_SOCKET) {
				throw std::runtime_error("exit");
			}
			expectedSocket = s | 0x10000;
			return expectedSocket;
		};
		bool invoked = false;
		Sockets::OnReceive = [&](SOCKET s, char* buf, int len, int flags) {
			if (expectedSocket != s || flags != 0) {
				return -1;
			}
			if (invoked) {
				return 0;
			}
			constexpr int size = _countof(request) - 1;
			if (len < size) {
				return -1;
			}
			invoked = true;
			memcpy_s(buf, len, request, size);
			return size;
		};
		std::string actualRequest;
		Dispatch::OnDispatch = [&actualRequest](ptr_t begin, ptr_t body, char*& next, ptr_t end, TcpSocket& client) {
			actualRequest.assign(begin, body);
		};
		StaticHttpServer server;

		// Act
		server.Run(6006);

		// Assert
		Assert::AreEqual(std::string(request), actualRequest);
	}
	};
}
