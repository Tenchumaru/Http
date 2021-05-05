#pragma once

#include "FibrousTcpSocket.h"

class SecureFibrousTcpSocket : public FibrousTcpSocket {
public:
	SecureFibrousTcpSocket(SOCKET socket, fn_t awaitFn, SSL_CTX* sslContext, bool isServer);
	SecureFibrousTcpSocket() = delete;
	SecureFibrousTcpSocket(SecureFibrousTcpSocket const&) = delete;
	SecureFibrousTcpSocket(SecureFibrousTcpSocket&& that) noexcept;
	SecureFibrousTcpSocket(FibrousTcpSocket&& that, SSL_CTX* sslContext, bool isServer);
	SecureFibrousTcpSocket& operator=(SecureFibrousTcpSocket const&) = delete;
	SecureFibrousTcpSocket& operator=(SecureFibrousTcpSocket&& that) noexcept;
	~SecureFibrousTcpSocket();

protected:
	int InternalReceive(char* buffer, size_t bufferSize) override;
	int InternalSend(char const* buffer, size_t bufferSize) override;

private:
	static BIO_METHOD* bioMethod;
	SSL* ssl{};

	static int BioRead(BIO* bio, char* data, size_t n, size_t* pn);
	static int BioWrite(BIO* bio, char const* data, size_t n, size_t* pn);
	void SwapPrivates(SecureFibrousTcpSocket& that);
};
