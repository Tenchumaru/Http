#include "pch.h"
#include "ClosableResponse.h"
#include "HttpParser.h"
#include "HttpServer.h"

namespace {
	std::array<char, 4> pattern = { '\r', '\n', '\r', '\n' };
	std::boyer_moore_searcher searcher(pattern.begin(), pattern.end());
}

void HttpServer::InternalHandleImpl(std::unique_ptr<FibrousTcpSocket> clientSocket) {
	std::array<char, 0x3800> buffer;
	auto* const begin = buffer.data();
	auto const size = buffer.size();
	auto* const end = begin + size;
	for (auto* next = begin;;) {
		// Check if the buffer contains at least the start line and the headers.
		auto* const body = std::search(begin, next, searcher);
		if (body != next) {
			// Dispatch the request with the client socket and a response object.  The
			// dispatcher is responsible for creating a request object containing the
			// start line, the headers, and the part of the body read so far.  It will
			// read the rest of the body as the application requests it and before closing
			// the response.
			auto const* p = ProcessRequest(begin, body + pattern.size(), next, *clientSocket);
			if (!p) {
				// The request specified closing the connection.
				return;
			}

			// Reset the buffer, copying any unprocessed data to its beginning.
			memmove_s(begin, size, p, next - p);
			next = begin;
		} else {
			// Read more data from the client socket.
			auto [n, errorCode] = clientSocket->Receive(next, end - next);
			if (n == 0 || errorCode) {
				return;
			}
			next += n;
		}
	}
}

char const* HttpServer::ProcessRequest(char const* begin, char const* body, char const* end, TcpSocket& clientSocket) const {
	// begin through body contains the start line and headers.  body through end
	// contains none, some, or all of the body.  If it contains all of the body, it
	// is followed by zero or more full requests followed by nothing or a partial request.
	std::array<char, 0x3800> buffer;
	ClosableResponse response(clientSocket, buffer.data(), buffer.data() + buffer.size());

	// Process the first request.  It also checks the Connection header value and
	// returns nullptr to indicate closing the socket.
	StatusLines::StatusLine statusLine{};
	char const* message = nullptr;
	try {
		end = DispatchRequest(begin, body, end, clientSocket, response);
		response.Close();
	} catch (HttpParser::Exception const& ex) {
		statusLine = ex.StatusLine;
		message = ex.Message;
	} catch (std::exception const& ex) {
		statusLine = StatusLines::InternalServerError;
		std::cerr << "DispatchRequest threw an exception:  " << ex.what() << std::endl;
	}
	if (statusLine != StatusLines::StatusLine{}) {
		// Respond with the appropriate status code if possible.
		if (response.Reset()) {
			response.WriteStatusLine(statusLine);
			if (message) {
				response.WriteHeader("Content-Length", std::to_string(strlen(message) + 2));
				response << message << "\r\n";
			}
			response.Close();
		}

		// Close the connection.
		return nullptr;
	}

	// If returning a non-null value and there are subsequent requests, the return
	// value will be the beginning of the next request if the socket received any.
	return end;
}
