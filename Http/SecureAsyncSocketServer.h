#pragma once

#include "AsyncSocketServer.h"

class SecureAsyncSocketServer : public AsyncSocketServer {
public:
	SecureAsyncSocketServer() = default;
	SecureAsyncSocketServer(SecureAsyncSocketServer const&) = delete;
	SecureAsyncSocketServer(SecureAsyncSocketServer&&) noexcept = default;
	SecureAsyncSocketServer& operator=(SecureAsyncSocketServer const&) = delete;
	SecureAsyncSocketServer& operator=(SecureAsyncSocketServer&&) noexcept = default;
	~SecureAsyncSocketServer() = default;
	void ConfigureSecurity(char const* certificateChainFile, char const* privateKeyFile);

private:
	struct SslContextDeleter {
		void operator()(SSL_CTX* p) const { SSL_CTX_free(p); };
	};

	std::unique_ptr<SSL_CTX, SslContextDeleter> sslContext;

	Task<std::pair<std::unique_ptr<AsyncSocket>, int>> Accept(SOCKET serverSocket, socklen_t addressSize) override;
};
