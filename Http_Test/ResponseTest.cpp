#include "stdafx.h"
#include "Socket.h"
#include "../Http/TcpSocket.h"
#include "../Http/Response.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace Http_Test {
	TEST_CLASS(ResponseTest) {
public:

	TEST_METHOD_INITIALIZE(Initialize) {
		Sockets::Initialize();
	}

	TEST_METHOD(TestResponseOk) {
		// Arrange
		bool invoked= false;
		SOCKET actualSocket= INVALID_SOCKET;
		int actualFlags= -1;
		std::string text;
		Sockets::OnSend= [&](SOCKET s, char const* p, int len, int flags) {
			if(p == nullptr) {
				return -1;
			}
			invoked= true;
			actualSocket= s;
			actualFlags= flags;
			text.assign(p, len);
			return len;
		};
		TcpSocket socket(value);
		Response response;

		// Act
		response.Ok("okay");
		response.Send(socket);

		// Assert
		Assert::IsTrue(invoked);
		Assert::AreEqual(value, actualSocket);
		Assert::AreEqual(0, actualFlags);
		Assert::AreEqual(0ull, text.find("HTTP/1.1 200 OK"));
		Assert::AreNotEqual(text.npos, text.find("Content-Length: 4\r\n"));
		Assert::AreEqual("okay", text.substr(text.size() - 4).c_str());
	}

	TEST_METHOD(TestResponseEnd) {
		// Arrange
		std::string text;
		Sockets::OnSend= [&text](SOCKET s, char const* p, int len, int flags) {
			text.assign(p, len);
			return len;
		};
		TcpSocket socket(value);
		Response response;

		// Act
		response.End(204);
		response.Send(socket);

		// Assert
		Assert::AreEqual(0ull, text.find("HTTP/1.1 204 No Content"));
		Assert::AreNotEqual(text.npos, text.find("Content-Length: 0\r\n"));
		Assert::AreEqual("\r\n\r\n", text.substr(text.size() - 4).c_str());
	}

private:
	SOCKET value= 110;
	};
}
