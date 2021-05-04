#pragma once

#include "AsyncSocket.h"

class SecureAsyncSocket : public AsyncSocket {
public:
	SecureAsyncSocket(AsyncSocket&& socket, SSL_CTX* sslContext);
	SecureAsyncSocket() = delete;
	SecureAsyncSocket(SecureAsyncSocket const&) = delete;
	SecureAsyncSocket(SecureAsyncSocket&& that) noexcept;
	SecureAsyncSocket& operator=(SecureAsyncSocket const&) = delete;
	SecureAsyncSocket& operator=(SecureAsyncSocket&& that) noexcept;
	~SecureAsyncSocket();
	Task<int> Accept();
	Task<std::pair<size_t, int>> Receive(void* buffer, size_t bufferSize) {
		return InternalReceive(buffer, bufferSize);
	}
	Task<std::pair<size_t, int>> Send(void const* buffer, size_t bufferSize) {
		return InternalSend(buffer, bufferSize);
	}
	Task<int> ShutDown();

protected:
	Task<std::pair<size_t, int>> InternalReceive(void* buffer, size_t bufferSize) override;
	Task<std::pair<size_t, int>> InternalSend(void const* buffer, size_t bufferSize) override;
	Task<int> InternalShutDown();

private:
	SSL* ssl{};

	void SwapPrivates(SecureAsyncSocket& that);
};

