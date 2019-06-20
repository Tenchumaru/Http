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

	void ConfigureSecurity(char const* certificateChainFile, char const* privateKeyFile);

private:
#ifdef _WIN32
	using InvokeFn = LPFIBER_START_ROUTINE;
#else
	using InvokeFn = void(*)(void*);
#endif

	std::vector<void*> availableFibers;
	std::function<void(SOCKET, short)> awaitFn;
	InvokeFn invokeOnConnectFn = &FibrousTcpSocketFactory::InvokeOnConnect;
	fn_t onConnect;
	void* mainFiber;
	SSL_CTX* sslContext = nullptr;
	SOCKET client;

	void Accept(SOCKET server, socklen_t sock_addr_size, fn_t onConnect) override;
	static void InvokeOnConnect(void* parameter);
	static void InvokeSecureOnConnect(void* parameter);

	void InvokeOnConnect(TcpSocket&& tcpSocket) {
		for(;;) {
			onConnect(std::move(tcpSocket));
			availableFibers.push_back(GetCurrentFiber());
			SwitchToFiber(mainFiber);
		}
	}
};
