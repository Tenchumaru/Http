#include "stdafx.h"
#include "TcpSocket.h"
#include "DispatchParser.h"
#include "ClosableResponse.h"
#include "StaticHttpServer.h"

TcpSocketFactory::fn_t StaticHttpServer::GetConnectFn() const {
	return [this](TcpSocket&& client) {
		// Give it to the dispatch parser.
		try {
			DispatchParser::DispatchParser(client);
		} catch(std::exception const& /*ex*/) {
			// Send a 500 Internal Server Error status code.
			char buffer[512];
			ClosableResponse response(client, buffer, buffer + _countof(buffer));
			response.Close();

			// Close the connection.
		}
	};
}
