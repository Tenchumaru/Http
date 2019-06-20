#pragma once

#include "TcpSocketFactory.h"

class FibrousTcpSocketFactory : public TcpSocketFactory {
public:
	FibrousTcpSocketFactory() = default;
	FibrousTcpSocketFactory(FibrousTcpSocketFactory const&) = delete;
	FibrousTcpSocketFactory(FibrousTcpSocketFactory&&) = default;
	FibrousTcpSocketFactory& operator=(FibrousTcpSocketFactory const&) = delete;
	FibrousTcpSocketFactory& operator=(FibrousTcpSocketFactory&&) = default;
	~FibrousTcpSocketFactory() = default;

private:
	std::vector<void*> availableFibers;
	std::function<void(SOCKET, short)> awaitFn;
	fn_t onConnect;
	void* mainFiber;
	SOCKET client;

	void Accept(SOCKET server, socklen_t sock_addr_size, fn_t onConnect) override;
	static void InvokeOnConnect(void* parameter);
};
