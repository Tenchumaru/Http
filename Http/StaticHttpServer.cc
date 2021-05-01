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
}
