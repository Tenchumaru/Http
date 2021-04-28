#include "pch.h"
#include "AsyncSocket.h"
#include "DispatchParser.h"
#include "ClosableAsyncResponse.h"
#include "StaticHttpServer.h"

Task<void> StaticHttpServer::Handle(std::unique_ptr<AsyncSocket> socket) {
	try {
		co_await DispatchParser::Async(*socket);
		co_return;
	} catch (std::exception const& /*ex*/) {
		// Handle the exception below.
	}

	// Send a 500 Internal Server Error status code.
	std::array<char, 4444> buffer;
	ClosableAsyncResponse response(*socket, buffer.data(), buffer.data() + buffer.size());
	co_await response.Close();
}
