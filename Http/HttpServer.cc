#include "stdafx.h"
#include "TcpSocketFactory.h"
#include "RequestParser.h"
#include "HttpServer.h"

// TODO:  consider an alternate design in which the Request object produced by
// a RequestParser has its handler invoked on a new fiber so the parser can
// continue, possibly parsing and producing the next request if the first
// becomes blocked.  However, since it's all the same socket, the client will
// expect the responses in the same order as the requests.  Buffer out-of-order
// responses until the preceding responses are sent.

HttpServer::HttpServer() {}

HttpServer::~HttpServer() {}

void HttpServer::Add(char const* path, fn_t fn) {
	handlers.push_back({ path, fn });
}

void HttpServer::Listen(unsigned short port) {
	auto connectFn= [this](TcpSocket&& client) {
		auto handlerFn= [this, &client](Request const& request) {
			auto const& path= request.Uri.Path;

			// Determine which handler to invoke.
			auto it= std::find_if(handlers.cbegin(), handlers.cend(), [path](auto const& p) {
				return strncmp(p.first.c_str(), path.c_str(), p.first.size()) == 0;
			});

			// Create the response.
			Response response;
			if(it != handlers.cend()) {
				// Invoke the handler.
				it->second(request, response);
			} else {
				// Return a 404.
				response.End(404);
			}

			// Send the response to the client.
			response.Send(client);
		};

		char buf[1024];
		RequestParser parser(handlerFn);
		for(;;) {
			// Read some data from the client.
			auto n= client.Receive(buf, sizeof(buf));
			if(n == 0) {
				break;
			}

			// Give it to the response parser.
			parser.Add(buf, n);
		}
	};
	TcpSocketFactory server;
	server.CreateServer(port, connectFn);
}
