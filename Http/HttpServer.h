#pragma once

#include <SecureFibrousSocketFactory.h>
#include "Date.h"
#include "Response.h"

class HttpServer {
public:
	HttpServer() = default;
	HttpServer(HttpServer const&) = delete;
	HttpServer& operator=(HttpServer const&) = delete;
	virtual ~HttpServer() = default;
	void ConfigureSecurity(char const* certificateChainFile, char const* privateKeyFile);
	int Run(std::uint16_t port);

protected:
	SecureFibrousSocketFactory factory;
	Date date;

private:
	void HandleClient(std::unique_ptr<ClientSocket>&& clientSocket);
	char const* ProcessRequest(char const* begin, char const* body, char const* end, ClientSocket& clientSocket) const;
	virtual char const* DispatchRequest(char const* begin, char const* body, char const* end, ClientSocket& clientSocket, Response& response) const = 0;
};
