#pragma once

#include "FibrousTcpSocket.h"

class SecureFibrousTcpSocket : public FibrousTcpSocket {
public:
	SecureFibrousTcpSocket(SOCKET socket, fn_t awaitFn, SSL_CTX* sslContext);
	SecureFibrousTcpSocket() = delete;
	SecureFibrousTcpSocket(SecureFibrousTcpSocket const&) = delete;
	SecureFibrousTcpSocket(SecureFibrousTcpSocket&& that) noexcept;
	SecureFibrousTcpSocket(FibrousTcpSocket&& that, SSL_CTX* sslContext);
	SecureFibrousTcpSocket& operator=(SecureFibrousTcpSocket const&) = delete;
	SecureFibrousTcpSocket& operator=(SecureFibrousTcpSocket&& that) noexcept;
	~SecureFibrousTcpSocket();
	int Accept() noexcept;
	int Connect(sockaddr const* address, size_t addressSize) noexcept override;

protected:
	std::pair<size_t, int> InternalReceive(char* buffer, size_t bufferSize) override;
	std::pair<size_t, int> InternalSend(char const* buffer, size_t bufferSize) override;

private:
	static BIO_METHOD* bioMethod;
	SSL* ssl{};

	int Invoke(std::function<int()> fn);
	void SwapPrivates(SecureFibrousTcpSocket& that);
};
