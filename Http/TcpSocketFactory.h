#pragma once

#include "TcpSocket.h"

class TcpSocketFactory {
public:
	using fn_t= std::function<void(std::shared_ptr<TcpSocket>)>;

	TcpSocketFactory();
	virtual ~TcpSocketFactory();
	void CreateClient(char const* hostName, unsigned short port, fn_t onConnect);
	void CreateClient(char const* hostName, char const* service, fn_t onConnect);
	void CreateServer(unsigned short port, fn_t onConnect);
	void CreateServer(char const* service, fn_t onConnect);

private:
	virtual void Accept(SOCKET server, socklen_t sock_addr_size, fn_t onConnect);
};
