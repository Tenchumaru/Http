#pragma once

#include "FibrousTcpSocket.h"

class SecureFibrousTcpSocket : public FibrousTcpSocket {
public:
	SecureFibrousTcpSocket(SOCKET socket, fn_t Await, bool isServer);
	SecureFibrousTcpSocket() = delete;
	SecureFibrousTcpSocket(SecureFibrousTcpSocket const&) = delete;
	SecureFibrousTcpSocket(SecureFibrousTcpSocket&&) = default;
	SecureFibrousTcpSocket& operator=(SecureFibrousTcpSocket const&) = delete;
	SecureFibrousTcpSocket& operator=(SecureFibrousTcpSocket&&) = default;
	~SecureFibrousTcpSocket();

	static void Configure(char const* certificateChainFile, char const* privateKeyFile);

protected:
	int InternalReceive(char* buffer, size_t bufferSize) override;
	int InternalSend(char const* buffer, size_t bufferSize) override;

private:
	static BIO_METHOD* bioMethod;
	SSL *ssl;

	static int BioRead(BIO* bio, char* data, size_t n, size_t* pn);
	static int BioWrite(BIO* bio, char const* data, size_t n, size_t* pn);
};
