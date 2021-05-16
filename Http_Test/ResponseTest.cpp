#include "pch.h"
#include "Sockets.h"
#include "../Http/Http.h"
#include "../Http/TcpSocket.h"
#include "../Http/ClosableResponse.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace Http_Test {
	TEST_CLASS(ResponseTest) {
public:

	TEST_METHOD_INITIALIZE(Initialize) {
		Sockets::Initialize();
	}

	TEST_METHOD(ResponseClose) {
		// Arrange
		bool invoked = false;
		SOCKET actualSocket = INVALID_SOCKET;
		int actualFlags = -1;
		std::string text;
		Sockets::OnSend = [&](SOCKET s, char const* p, int len, int flags) {
			if (p == nullptr) {
				return -1;
			}
			invoked = true;
			actualSocket = s;
			actualFlags = flags;
			text.append(p, len);
			return len;
		};
		TcpSocket socket(expectedSocket);
		std::array<char, Response::MinimumBufferSize> buffer;
		ClosableResponse response(socket, buffer.data(), buffer.data() + buffer.size());

		// Act
		response.Close();

		// Assert
		Assert::IsFalse(invoked);
	}

	TEST_METHOD(ResponseWriteStatusLine) {
		// Arrange
		std::string text;
		Sockets::OnSend = [&text](SOCKET s, char const* p, int len, int flags) {
			if (p == nullptr) {
				return -1;
			}
			text.append(p, len);
			return len;
		};
		TcpSocket socket(expectedSocket);
		std::array<char, Response::MinimumBufferSize> buffer;
		ClosableResponse response(socket, buffer.data(), buffer.data() + buffer.size());

		// Act
		response.WriteStatusLine(StatusLines::NoContent);
		response.Close();

		// Assert
		Assert::AreEqual(0ull, text.find("HTTP/1.1 204 No Content\r\n"));
		Assert::AreEqual("\r\n\r\n", text.substr(text.size() - 4).c_str());
		Assert::IsFalse(std::isspace(text[text.size() - 5], std::locale()));
	}

	TEST_METHOD(ResponseChunked) {
		// Arrange
		std::string text;
		Sockets::OnSend = [&text](SOCKET s, char const* p, int len, int flags) {
			if (p == nullptr) {
				return -1;
			}
			text.append(p, len);
			return len;
		};
		TcpSocket socket(expectedSocket);
		std::array<char, 3 * Response::MinimumBufferSize / 2> buffer;
		ClosableResponse response(socket, buffer.data(), buffer.data() + buffer.size());
		std::string m(333, '.');

		// Act
		response << m << m << m;
		response.Close();

		// Assert
		Assert::AreEqual(decltype(text.npos)(), text.find("HTTP/1.1 200 OK"));
		Assert::AreEqual(text.npos, text.find("Content-Length: "));
		Assert::AreNotEqual(text.npos, text.find("Transfer-Encoding: chunked\r\n"));
		std::stringstream responseBody;
		responseBody << "\r\n\r\n29a\r\n" << m << m << "\r\n14d\r\n" << m << "\r\n0\r\n\r\n";
		Assert::AreEqual(responseBody.str().c_str(), text.substr(text.size() - responseBody.str().size()).c_str());
	}

private:
	SOCKET expectedSocket = 110;
	};
}
