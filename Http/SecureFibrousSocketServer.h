#pragma once

#include "FibrousSocketServer.h"

class SecureFibrousSocketServer : public FibrousSocketServer {
public:
	SecureFibrousSocketServer() = default;
	SecureFibrousSocketServer(SecureFibrousSocketServer const&) = delete;
	SecureFibrousSocketServer(SecureFibrousSocketServer&&) noexcept = default;
	SecureFibrousSocketServer& operator=(SecureFibrousSocketServer const&) = delete;
	SecureFibrousSocketServer& operator=(SecureFibrousSocketServer&&) noexcept = default;
	~SecureFibrousSocketServer() override = default;
	void ConfigureSecurity(char const* certificateChainFile, char const* privateKeyFile);

protected:
	std::unique_ptr<TcpSocket> MakeSocketImpl(SOCKET socket) const override;

private:
	struct SslContextDeleter {
		void operator()(SSL_CTX* p) const { SSL_CTX_free(p); };
	};

	std::unique_ptr<SSL_CTX, SslContextDeleter> sslContext;

	void InternalHandle(std::unique_ptr<FibrousTcpSocket> clientSocket) final override;
	virtual void InternalHandleImpl(std::unique_ptr<FibrousTcpSocket> clientSocket) = 0;
};
