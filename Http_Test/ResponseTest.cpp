#include "pch.h"
#include "Sockets.h"
#include "../Http/Http.h"
#include "../Http/TcpSocket.h"
#include "../Http/ClosableResponse.h"

using namespace std::literals;
using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace Http_Test {
	TEST_CLASS(ResponseTest) {
public:
	TEST_METHOD_INITIALIZE(Initialize) {
		Sockets::Initialize();
		Sockets::OnSend = [this](SOCKET s, char const* p, int len, int flags) {
			if (p == nullptr || s != expectedSocket || flags) {
				return -1;
			}
			text.append(p, len);
			return len;
		};
	}

	TEST_METHOD(ResponseClose) {
		// Arrange
		TcpSocket socket{ expectedSocket };
		ClosableResponse response(date, socket, buffer.data(), buffer.data() + buffer.size());
		auto statusLine = StatusLines::NoContent;

		// Act
		response.Close();

		// Assert
		AssertServerHeaders(statusLine);
		Assert::AreEqual(size_t(77), text.size());
	}

	TEST_METHOD(ResponseWriteStatusLine) {
		// Arrange
		TcpSocket socket{ expectedSocket };
		ClosableResponse response(date, socket, buffer.data(), buffer.data() + buffer.size());
		auto statusLine = StatusLines::NoContent;

		// Act
		response.WriteStatusLine(statusLine);
		response.Close();

		// Assert
		AssertServerHeaders(statusLine);
		Assert::AreEqual(size_t(77), text.size());
	}

	TEST_METHOD(ResponseChunked) {
		// Arrange
		TcpSocket socket{ expectedSocket };
		ClosableResponse response(date, socket, buffer.data(), buffer.data() + buffer.size());
		std::string m(333, '.');
		auto statusLine = StatusLines::OK;

		// Act
		response << m << m << m;
		response.Close();

		// Assert
		AssertServerHeaders(statusLine);
		Assert::AreEqual(text.npos, text.find("Content-Length: "));
		Assert::AreNotEqual(text.npos, text.find("Transfer-Encoding: chunked\r\n"));
		std::stringstream ss;
		ss << "\r\n\r\n01F8\r\n" << std::string(0x01F8, '.') << "\r\n01EF\r\n" << std::string(0x01EF, '.') << "\r\n0\r\n\r\n";
		auto expectedResponseBody = ss.str();
		Assert::AreEqual(expectedResponseBody.c_str(), text.substr(text.size() - expectedResponseBody.size()).c_str());
	}

private:
	static constexpr SOCKET expectedSocket = 110;
	std::array<char, Response::MinimumBufferSize> buffer;
	Date date;
	std::string text;

	void AssertServerHeaders(std::string const& statusLine) {
		Assert::AreEqual(std::string::size_type(), text.find(statusLine));
		Assert::AreNotEqual(std::string::npos, text.find("Date: "));
		Assert::AreNotEqual(std::string::npos, text.find(" GMT\r\n"));
		Assert::AreNotEqual(std::string::npos, text.find("Server: C++\r\n"));
		Assert::AreEqual("\r\n\r\n"s, text.substr(text.size() - 4));
	}
	};
}
