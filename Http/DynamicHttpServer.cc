#include "pch.h"
#include "DynamicHttpServer.h"

using namespace std::literals;

void DynamicHttpServer::Add(char const* path, fn_t fn) {
	handlers.push_back({ path, fn });
}

char const* DynamicHttpServer::DispatchRequest(char const* begin, char const* body, char const* end, TcpSocket& socket, Response& response) const {
	auto* this_ = const_cast<std::remove_const_t<std::remove_pointer_t<decltype(this)>>*>(this);
	auto const* p = begin;
	do {
		p = this_->parser.Add(p, body);
	} while (!this_->parser.ComposeRequest(this_->request));
	auto const& path = request.Uri.Path;

	// Determine which handler to invoke.
	auto it = std::find_if(handlers.cbegin(), handlers.cend(), [path](auto const& p) {
		return strncmp(p.first.c_str(), path.c_str(), p.first.size()) == 0;
	});

	if (it != handlers.cend()) {
		// If the request contains an expectation, immediately respond with a continue.
		if (request.Headers.find("Expect"s) != request.Headers.cend()) {
			std::array<char, Response::MinimumBufferSize> continueBuffer;
			ClosableResponse continueResponse(date, socket, continueBuffer.data(), continueBuffer.data() + continueBuffer.size());
			continueResponse.WriteStatusLine(StatusLines::Continue);
			continueResponse.Close();
		}

		// Invoke the handler.
		auto it_ = request.Headers.find("Content-Length"s);
		auto size = it_ == request.Headers.cend() ? 0 : std::strtol(it_->second.c_str(), nullptr, 10);
		it->second(request, Body(body, end, size, socket), response);
	} else {
		// Return a 404.
		response.WriteStatusLine(StatusLines::NotFound);
	}

	// If the HTTP version is at least 1.1 and there is a "Connection"
	// header whose value is "Close", close the socket.  Otherwise, if
	// the HTTP version is less than 1.1 and there is no "Connection"
	// header whose value is "Keep-Alive", close the socket.
	bool isLastRequest;
	auto header = request.Headers.find("Connection");
	if (request.Version >= 0x11) {
		isLastRequest = header != request.Headers.cend() && _stricmp(header->second.c_str(), "close") == 0;
	} else {
		isLastRequest = header == request.Headers.cend() || _stricmp(header->second.c_str(), "keep-alive") != 0;
	}
	return isLastRequest ? nullptr : p;
}
