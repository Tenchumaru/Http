#include "stdafx.h"
#include "Sockets.h"
#include "../Http/TcpSocket.h"
#include "../Http/ClosableResponse.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace Http_Test {
	TEST_CLASS(ResponseTest) {
public:

	TEST_METHOD_INITIALIZE(Initialize) {
		Sockets::Initialize();
	}

	TEST_METHOD(ResponseOk) {
		// Arrange
		bool invoked = false;
		SOCKET actualSocket = INVALID_SOCKET;
		int actualFlags = -1;
		std::string text;
		Sockets::OnSend = [&](SOCKET s, char const* p, int len, int flags) {
			if(p == nullptr) {
				return -1;
			}
			invoked = true;
			actualSocket = s;
			actualFlags = flags;
			text.append(p, len);
			return len;
		};
		TcpSocket socket(expectedSocket);
		ClosableResponse response(socket);

		// Act
		response.Ok("okay");
		response.Close();

		// Assert
		Assert::IsTrue(invoked);
		Assert::AreEqual(expectedSocket, actualSocket);
		Assert::AreEqual(0, actualFlags);
		Assert::AreEqual(0ull, text.find("HTTP/1.1 200 OK"));
		Assert::AreNotEqual(text.npos, text.find("Content-Length: 4\r\n"));
		Assert::AreEqual("okay", text.substr(text.size() - 4).c_str());
	}

	TEST_METHOD(ResponseEnd) {
		// Arrange
		std::string text;
		Sockets::OnSend = [&text](SOCKET s, char const* p, int len, int flags) {
			if(p == nullptr) {
				return -1;
			}
			text.append(p, len);
			return len;
		};
		TcpSocket socket(expectedSocket);
		ClosableResponse response(socket);

		// Act
		response.End(204);
		response.Close();

		// Assert
		Assert::AreEqual(0ull, text.find("HTTP/1.1 204 No Content"));
		Assert::AreNotEqual(text.npos, text.find("Content-Length: 0\r\n"));
		Assert::AreEqual("\r\n\r\n", text.substr(text.size() - 4).c_str());
	}

	TEST_METHOD(ResponseChunked) {
		// Arrange
		std::string text;
		Sockets::OnSend = [&text](SOCKET s, char const* p, int len, int flags) {
			if(p == nullptr) {
				return -1;
			}
			text.append(p, len);
			return len;
		};
		TcpSocket socket(expectedSocket);
		ClosableResponse response(socket);
		std::string m(1111, '.');

		// Act
		response.ResponseStream << m << m << m;
		response.Close();

		// Assert
		Assert::AreEqual(0ull, text.find("HTTP/1.1 200 OK"));
		Assert::AreEqual(text.npos, text.find("Content-Length: 0\r\n"));
		Assert::AreNotEqual(text.npos, text.find("Transfer-Encoding: chunked\r\n"));
		std::stringstream responseBody;
		responseBody << "\r\n\r\n8ae\r\n" << m << m << "\r\n457\r\n" << m << "\r\n0\r\n\r\n";
		Assert::AreEqual(responseBody.str().c_str(), text.substr(text.size() - responseBody.str().size()).c_str());
	}

private:
	SOCKET expectedSocket = 110;
	};
}
