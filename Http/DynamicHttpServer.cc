#include "pch.h"
#include "Request.h"
#include "RequestParser.h"
#include "ClosableAsyncResponse.h"
#include "DynamicHttpServer.h"

void DynamicHttpServer::Add(char const* path, fn_t fn) {
	handlers.push_back({ path, fn });
}

Task<void> DynamicHttpServer::Handle(std::unique_ptr<AsyncSocket> clientSocket) {
	Request request;
	RequestParser parser;
	std::array<char, 0x400> buffer;
	for (;;) {
		// Read some data from the client.
		auto [n, errorCode] = co_await clientSocket->Receive(buffer.data(), buffer.size());
		if (n == 0 || errorCode) {
			co_return;
		}

		// Give it to the response parser.
		StatusLines::StatusLine statusLine{};
		try {
			char const* p = buffer.data();
			char const* const q = p + n;
			while (p < q) {
				p = parser.Add(p, q);
				while (parser.ComposeRequest(request)) {
					auto isLastRequest = co_await InternalHandle(request, clientSocket);
					if (isLastRequest) {
						co_return;
					}
				}
			}
		} catch (HttpParser::Exception const& ex) {
			statusLine = ex.StatusLine;
		} catch (std::exception const& /*ex*/) {
			statusLine = StatusLines::InternalServerError;
		}
		if (statusLine != StatusLines::StatusLine{}) {
			// Respond with the appropriate status code.
			ClosableAsyncResponse response(*clientSocket, buffer.data(), buffer.data() + buffer.size());
			response.WriteStatus(statusLine);
			co_await response.Close();

			// Close the connection.
			co_return;
		}
	}
}

Task<bool> DynamicHttpServer::InternalHandle(Request const& request, std::unique_ptr<AsyncSocket>& clientSocket) const {
	auto const& path = request.Uri.Path;

	// Determine which handler to invoke.
	auto it = std::find_if(handlers.cbegin(), handlers.cend(), [path](auto const& p) {
		return strncmp(p.first.c_str(), path.c_str(), p.first.size()) == 0;
	});

	// Create the response and send it to the client.
	// TODO:  consider creating a flushable response to allow for
	// content larger than this buffer.
	std::array<char, 16'000> buffer;
	ClosableAsyncResponse response(*clientSocket, buffer.data(), buffer.data() + buffer.size());

	// TODO:  consider requiring the "Content-Length" header and
	// responding with "411 Length Required" if it's missing.  However,
	// section 3.3.2 of RFC 7230 says user agents mustn't send this
	// header if there is no data and the method doesn't expect any so
	// check the request verb.

	if (it != handlers.cend()) {
		// Invoke the handler.
		co_await it->second(request, response);
	} else {
		// Return a 404.
		response.WriteStatus(StatusLines::NotFound);
	}
	co_await response.Close();

	// If the HTTP version is at least 1.1 and there is a "Connection"
	// header whose value is "Close", close the socket.  Otherwise, if
	// the HTTP version is less than 1.1 and there is no "Connection"
	// header whose value is "Keep-Alive", close the socket.
	bool isLastRequest;
	auto header = request.Headers.find("connection");
	if (request.Version >= 0x11) {
		isLastRequest = header != request.Headers.cend() && _stricmp(header->second.c_str(), "close") == 0;
	} else {
		isLastRequest = header == request.Headers.cend() || _stricmp(header->second.c_str(), "keep-alive") != 0;
	}
	co_return isLastRequest;
}
