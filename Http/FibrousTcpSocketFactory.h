#pragma once

#include "TcpSocketFactory.h"
#include "FibrousTcpSocket.h"

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
	using InvokeFn = std::function<void()>;
	struct SslContextDeleter {
		void operator()(SSL_CTX* p) const { SSL_CTX_free(p); };
	};

	std::vector<void*> availableFibers;
	std::function<void(SOCKET, short)> awaitFn;
	InvokeFn invokeOnConnectFn = [this] { return onConnect(FibrousTcpSocket(client, awaitFn)); };
	fn_t onConnect;
	void* mainFiber;
	std::unique_ptr<SSL_CTX, SslContextDeleter> sslContext;
	SOCKET client;

	void Accept(SOCKET server, socklen_t sock_addr_size, fn_t onConnect) override;
	static void InvokeOnConnect(void* parameter);
};
