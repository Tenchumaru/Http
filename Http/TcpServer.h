#pragma once

#include "Socket.h"

class TcpServer {
public:
	using fn_t= std::function<void(Socket)>;

	explicit TcpServer(fn_t fn);
	virtual ~TcpServer();
	void Listen(unsigned short port);
	void Listen(char const* service);

protected:
	fn_t onConnect;

private:
	virtual void Accept(SOCKET server, socklen_t sock_addr_size);
};
