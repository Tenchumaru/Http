#include "stdafx.h"
#include "FibrousTcpSocketFactory.h"
#include "RequestParser.h"
#include "ClosableResponse.h"
#include "HttpServer.h"

// TODO:  consider an alternate design in which the Request object produced by
// a RequestParser has its handler invoked on a new fiber so the parser can
// continue, possibly parsing and producing the next request if the first
// becomes blocked.  However, since it's all the same socket, the client will
// expect the responses in the same order as the requests.  Buffer out-of-order
// responses until the preceding responses are sent.  Additionally, if a later
// request depends on the outcome of an earlier request, this will necessitate
// synchronization to ensure effects occur in the expected order.

HttpServer::HttpServer() {}

HttpServer::~HttpServer() {}

void HttpServer::Add(char const* path, fn_t fn) {
	handlers.push_back({ path, fn });
}

void HttpServer::Listen(unsigned short port) {
	auto connectFn = [this](TcpSocket&& client) {
		auto handlerFn = [this, &client](Request const& request) {
			auto const& path = request.Uri.Path;

			// Determine which handler to invoke.
			auto it = std::find_if(handlers.cbegin(), handlers.cend(), [path](auto const& p) {
				return strncmp(p.first.c_str(), path.c_str(), p.first.size()) == 0;
			});

			// Create the response and send it to the client.
			ClosableResponse response(client);

			// TODO:  consider requiring the "Content-Length" header and
			// responding with "411 Length Required" if it's missing.  However,
			// section 3.3.2 of RFC 7230 says user agents mustn't send this
			// header if there is no data and the method doesn't expect any so
			// check the request verb.

			if(it != handlers.cend()) {
				// Invoke the handler.
				it->second(request, response);
			} else {
				// Return a 404.
				response.End(404);
			}
			response.Close();

			// If the HTTP version is at least 1.1 and there is a "Connection"
			// header whose value is "Close", close the socket.  Otherwise, if
			// the HTTP version is less than 1.1 and there is no "Connection"
			// header whose value is "Keep-Alive", close the socket.
			bool wantsClose;
			auto header = request.Headers.find("connection");
			if(request.Version >= 0x11) {
				wantsClose = header != request.Headers.cend() && _stricmp(header->second.c_str(), "close") == 0;
			} else {
				wantsClose = header == request.Headers.cend() || _stricmp(header->second.c_str(), "keep-alive") != 0;
			}
			return wantsClose;
		};

		char buf[1024];
		RequestParser parser(handlerFn);
		for(;;) {
			// Read some data from the client.
			auto n = client.Receive(buf, sizeof(buf));
			if(n == 0) {
				break;
			} else if(n < 0) {
				auto v = errno;
				if(v == EALREADY || v == EWOULDBLOCK) {
					continue;
				} else {
					break;
				}
			}

			// Give it to the response parser.
			try {
				if(parser.Add(buf, n)) {
					break;
				}
			} catch(HttpParser::Exception const& ex) {
				// Respond with the appropriate status code.
				ClosableResponse response(client);
				response.End(ex.StatusCode < 600 ? ex.StatusCode : 500);
				response.Close();

				// Close the connection.
				break;
			} catch(std::exception const& /*ex*/) {
				// Send a 500 Internal Server Error status code.
				ClosableResponse response(client);
				response.End(500);
				response.Close();

				// Close the connection.
				break;
			}
		}
	};
	FibrousTcpSocketFactory server;
	server.CreateServer(port, connectFn);
}
