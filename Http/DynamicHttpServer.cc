#include "pch.h"
#include "TcpSocketFactory.h"
#include "Request.h"
#include "RequestParser.h"
#include "ClosableResponse.h"
#include "DynamicHttpServer.h"

void DynamicHttpServer::Add(char const* path, fn_t fn) {
	handlers.push_back({ path, fn });
}

TcpSocketFactory::fn_t DynamicHttpServer::GetConnectFn() const {
	return [this](TcpSocket&& socket) {
		auto handlerFn = [this, &socket](Request const& request) {
			auto const& path = request.Uri.Path;

			// Determine which handler to invoke.
			auto it = std::find_if(handlers.cbegin(), handlers.cend(), [path](auto const& p) {
				return strncmp(p.first.c_str(), path.c_str(), p.first.size()) == 0;
			});

			// Create the response and send it to the client.
			// TODO:  consider creating a flushable response to allow for
			// content larger than this buffer.
			std::array<char, 0x10000> buffer;
			ClosableResponse response(socket, buffer.data(), buffer.data() + buffer.size());

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
			response.Close();

			// If the HTTP version is at least 1.1 and there is a "Connection"
			// header whose value is "Close", close the socket.  Otherwise, if
			// the HTTP version is less than 1.1 and there is no "Connection"
			// header whose value is "Keep-Alive", close the socket.
			bool wantsClose;
			auto header = request.Headers.find("connection");
			if (request.Version >= 0x11) {
				wantsClose = header != request.Headers.cend() && _stricmp(header->second.c_str(), "close") == 0;
			} else {
				wantsClose = header == request.Headers.cend() || _stricmp(header->second.c_str(), "keep-alive") != 0;
			}
			return wantsClose;
		};

		char buf[1024];
		RequestParser parser(handlerFn);
		for (;;) {
			// Read some data from the client.
			auto n = socket.Receive(buf, sizeof(buf));
			if (n == 0) {
				break;
			} else if (n < 0) {
				auto v = errno;
				if (v == EALREADY || v == EWOULDBLOCK) {
					continue;
				} else {
					break;
				}
			}

			// Give it to the response parser.
			try {
				if (parser.Add(buf, n)) {
					break;
				}
			} catch (HttpParser::Exception const& ex) {
				// Respond with the appropriate status code.
				std::array<char, 0x400> buffer;
				ClosableResponse response(socket, buffer.data(), buffer.data() + buffer.size());
				response.WriteStatus(ex.StatusLine);
				response.Close();

				// Close the connection.
				break;
			} catch (std::exception const& /*ex*/) {
				// Send a 500 Internal Server Error status code.
				std::array<char, 0x400> buffer;
				ClosableResponse response(socket, buffer.data(), buffer.data() + buffer.size());
				response.WriteStatus(StatusLines::InternalServerError);
				response.Close();

				// Close the connection.
				break;
			}
		}
	};
}
