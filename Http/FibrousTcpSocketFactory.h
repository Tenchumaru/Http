#pragma once

#include "TcpSocketFactory.h"

class FibrousTcpSocketFactory : public TcpSocketFactory {
public:
	FibrousTcpSocketFactory();
	~FibrousTcpSocketFactory();

private:
	fn_t onConnect;
	void* mainFiber;
	SOCKET client;

	void Accept(SOCKET server, socklen_t sock_addr_size, fn_t onConnect) override;
	static void InvokeOnConnect(void* parameter);
};
