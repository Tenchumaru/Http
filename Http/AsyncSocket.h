#pragma once

#include "Socket.h"
#include "Task.h"

class AsyncSocket : public Socket {
public:
	explicit AsyncSocket(SOCKET socket);
	AsyncSocket() = delete;
	AsyncSocket(AsyncSocket const&) = delete;
	AsyncSocket(AsyncSocket&&) noexcept = default;
	AsyncSocket& operator=(AsyncSocket const&) = delete;
	AsyncSocket& operator=(AsyncSocket&&) noexcept = default;
	~AsyncSocket() = default;
	Task<std::pair<size_t, int>> Receive(void* buffer, size_t bufferSize) {
		return InternalReceive(buffer, bufferSize);
	}
	Task<std::pair<size_t, int>> Send(void const* buffer, size_t bufferSize) {
		return InternalSend(buffer, bufferSize);
	}

protected:
	virtual Task<std::pair<size_t, int>> InternalReceive(void* buffer, size_t bufferSize);
	virtual Task<std::pair<size_t, int>> InternalSend(void const* buffer, size_t bufferSize);
};
