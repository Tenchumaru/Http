#include "pch.h"
#include "DynamicHttpServer.h"

void DynamicHttpServer::Add(char const* path, fn_t fn) {
	handlers.push_back({ path, fn });
}

bool DynamicHttpServer::Dispatch(TcpSocket& socket, ClosableResponse& response) {
	// Read some data from the client.
	std::array<char, 0x1000> buffer;
	auto n = socket.Receive(buffer.data(), buffer.size());
	if (n <= 0) {
		return true;
	}

	// Give it to the response parser.
	StatusLines::StatusLine statusLine{};
	try {
		char const* p = buffer.data();
		char const* const q = p + n;
		while (p < q) {
			p = parser.Add(p, q);
			while (parser.ComposeRequest(request)) {
				auto isLastRequest = InternalHandle(response);
				if (isLastRequest) {
					return true;
				}
				response.Close();
			}
		}
	} catch (HttpParser::Exception const& ex) {
		statusLine = ex.StatusLine;
	} catch (std::exception const& /*ex*/) {
		statusLine = StatusLines::InternalServerError;
	}
	if (statusLine != StatusLines::StatusLine{}) {
		// Respond with the appropriate status code.
		response.WriteStatus(statusLine);

		// Close the connection.
		return true;
	}
	return false;
}

char const* DynamicHttpServer::DispatchRequest(char const* begin, char const* body, char* next, char const* end, TcpSocket& socket, Response& response) const {
	next, end, socket;
	auto* this_ = const_cast<std::remove_const_t<std::remove_pointer_t<decltype(this)>>*>(this);
	auto const* p = begin;
	do {
		p = this_->parser.Add(p, body);
	} while (!this_->parser.ComposeRequest(this_->request));
	auto isLastRequest = InternalHandle(response);
	return isLastRequest ? nullptr : p;
}

bool DynamicHttpServer::InternalHandle(Response& response) const {
	auto const& path = request.Uri.Path;

	// Determine which handler to invoke.
	auto it = std::find_if(handlers.cbegin(), handlers.cend(), [path](auto const& p) {
		return strncmp(p.first.c_str(), path.c_str(), p.first.size()) == 0;
	});

	// TODO:  consider requiring the "Content-Length" header and
	// responding with "411 Length Required" if it's missing.  However,
	// section 3.3.2 of RFC 7230 says user agents mustn't send this
	// header if there is no data and the method doesn't expect any so
	// check the request verb.

	if (it != handlers.cend()) {
		// Invoke the handler.
		it->second(request, response);
	} else {
		// Return a 404.
		response.WriteStatus(StatusLines::NotFound);
	}

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
	return isLastRequest;
}
