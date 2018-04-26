#include "stdafx.h"
#include "Sockets.h"
#include "../Http/HttpServer.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace Http_Test {
	TEST_CLASS(HttpServerTest) {
public:

	TEST_METHOD_INITIALIZE(Initialize) {
		Sockets::Initialize();
	}

	TEST_METHOD(HttpServerListenWithHandler) {
		// Arrange
		SOCKET expectedSocket= INVALID_SOCKET;
		Sockets::OnAccept= [&](SOCKET s, sockaddr* addr, int* addrlen) {
			if(addr == nullptr || addrlen == nullptr) {
				return INVALID_SOCKET;
			}
			if(expectedSocket != INVALID_SOCKET) {
				throw std::runtime_error("exit");
			}
			expectedSocket= s | 0x10000;
			return expectedSocket;
		};
		bool invoked= false;
		Sockets::OnReceive= [&](SOCKET s, char* buf, int len, int flags) {
			if(expectedSocket != s || flags != 0 || invoked) {
				return -1;
			}
			char const request[]= "GET /f/15 HTTP/1.1\r\n"
				"Host: localhost:6006\r\n"
				"User-Agent: Mozilla/5.0 (Windows NT 10.0; Win64; x64; rv:59.0) Gecko/20100101 Firefox/59.0\r\n"
				"Accept: text/html,application/xhtml+xml,application/xml;q=0.9,*/*;q=0.8\r\n"
				"Accept-Language: en-US,en;q=0.5\r\n"
				"Accept-Encoding: gzip, deflate\r\n"
				"DNT: 1\r\n"
				"Connection: keep-alive\r\n"
				"Upgrade-Insecure-Requests: 1\r\n"
				"\r\n";
			int const size= _countof(request) - 1;
			char ch= request[size]; ch;
			if(len < size) {
				return -1;
			}
			invoked= true;
			memcpy_s(buf, len, request, size);
			return size;
		};
		std::string response;
		Sockets::OnSend= [&](SOCKET s, char const* buf, int len, int flags) {
			if(expectedSocket != s || flags != 0 || !response.empty()) {
				return -1;
			}
			response.assign(buf, len);
			return len;
		};
		std::string path;
		auto fn= [&path](Request const& request, Response& response) {
			path= request.Uri.Path;
			response.Ok("okay");
		};
		HttpServer server;
		server.Add("/f/", fn);

		// Act
		server.Listen(6006);

		// Assert
		Assert::AreEqual("/f/15", path.c_str());
		Assert::AreEqual(0ull, response.find("HTTP/1.1 200 OK"));
		Assert::AreNotEqual(response.npos, response.find("Content-Length: 4\r\n"));
		Assert::AreEqual("okay", response.substr(response.size() - 4).c_str());
	}

	TEST_METHOD(HttpServerListenWithoutHandler) {
		// Arrange
		SOCKET expectedSocket= INVALID_SOCKET;
		Sockets::OnAccept= [&](SOCKET s, sockaddr* addr, int* addrlen) {
			if(addr == nullptr || addrlen == nullptr) {
				return INVALID_SOCKET;
			}
			if(expectedSocket != INVALID_SOCKET) {
				throw std::runtime_error("exit");
			}
			expectedSocket= s | 0x10000;
			return expectedSocket;
		};
		bool invoked= false;
		Sockets::OnReceive= [&](SOCKET s, char* buf, int len, int flags) {
			if(expectedSocket != s || flags != 0 || invoked) {
				return -1;
			}
			char const request[]= "GET /f/15 HTTP/1.1\r\n"
				"Host: localhost:6006\r\n"
				"User-Agent: Mozilla/5.0 (Windows NT 10.0; Win64; x64; rv:59.0) Gecko/20100101 Firefox/59.0\r\n"
				"Accept: text/html,application/xhtml+xml,application/xml;q=0.9,*/*;q=0.8\r\n"
				"Accept-Language: en-US,en;q=0.5\r\n"
				"Accept-Encoding: gzip, deflate\r\n"
				"DNT: 1\r\n"
				"Connection: keep-alive\r\n"
				"Upgrade-Insecure-Requests: 1\r\n"
				"\r\n";
			int const size= _countof(request) - 1;
			char ch= request[size]; ch;
			if(len < size) {
				return -1;
			}
			invoked= true;
			memcpy_s(buf, len, request, size);
			return size;
		};
		std::string response;
		Sockets::OnSend= [&](SOCKET s, char const* buf, int len, int flags) {
			if(expectedSocket != s || flags != 0 || !response.empty()) {
				return -1;
			}
			response.assign(buf, len);
			return len;
		};
		HttpServer server;

		// Act
		server.Listen(6006);

		// Assert
		Assert::AreEqual(0ull, response.find("HTTP/1.1 404 Not Found\r\n"));
		Assert::AreNotEqual(response.npos, response.find("Content-Length: 0\r\n"));
		Assert::AreEqual("\r\n\r\n", response.substr(response.size() - 4).c_str());
	}
	};
}
