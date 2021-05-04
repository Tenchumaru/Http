#pragma once

#include "xtypes.h"
#include "StatusLines.h"
#include "AsyncSocket.h"

class AsyncResponse {
public:
	AsyncResponse(AsyncSocket& socket, char* begin, char* end);
	AsyncResponse() = delete;
	AsyncResponse(AsyncResponse const&) = delete;
	AsyncResponse(AsyncResponse&&) noexcept = default;
	AsyncResponse& operator=(AsyncResponse const&) = delete;
	AsyncResponse& operator=(AsyncResponse&&) noexcept = default;
	template<typename T>
	Task<void> Write(T t) {
		inBody = true;
		std::stringstream ss;
		ss << t;
		return InternalWrite(ss.str());
	}
	template<typename T>
	Task<void> Write(std::vector<T>&& v) {
		return InternalWrite(std::move(v));
	}
	template<>
	Task<void> Write(std::string&& s) {
		return InternalWrite(std::move(s));
	}
	void WriteStatus(StatusLines::StatusLine const& statusLine);
	void WriteHeader(std::string const& name, std::string const& value) {
		WriteHeader(name, xstring{ value.data(), value.data() + value.size() });
	}
	void WriteHeader(std::string const& name, xstring const& value) {
		WriteHeader(xstring{ name.data(), name.data() + name.size() }, value);
	}
	void WriteHeader(xstring const& name, xstring const& value);

protected:
	// These are protected since derived classes control the response lifetime.
	~AsyncResponse() = default;
	Task<void> Close();

private:
	struct AsyncBuffer {
		AsyncBuffer(AsyncResponse& response, AsyncSocket& socket);
		AsyncBuffer() = delete;
		AsyncBuffer(AsyncBuffer const&) = delete;
		AsyncBuffer(AsyncBuffer&&) noexcept = default;
		AsyncBuffer& operator=(AsyncBuffer const&) = delete;
		AsyncBuffer& operator=(AsyncBuffer&&) noexcept = default;
		Task<void> Close();
		Task<void> Write(void const* s, size_t n);

	private:
		using Fn0 = Task<void>(AsyncBuffer::*)();
		using Fn2 = Task<void>(AsyncBuffer::*)(void const* s, size_t n);

		AsyncResponse& response;
		AsyncSocket& socket;
		Fn0 closeFn;
		Fn0 internalSendBufferFn;
		Fn2 internalSendFn;
		Fn2 sendFn;
		char* begin;
		char* next;
		char* end;

		Task<void> InitialSend(void const* s, size_t n);
		Task<void> Send(void const* s, size_t n);
		Task<void> ChunkedClose();
		Task<void> SimpleClose();
		Task<void> InternalSendBuffer();
		Task<void> InternalSend(void const* s, size_t n);
		Task<void> InternalSendBufferChunk();
		Task<void> InternalSendChunk(void const* s, size_t n);
	};

	char* begin{};
	char* next{};
	char* end{};
	AsyncBuffer buffer;
	bool wroteContentLength{};
	bool wroteServer{};
	bool inBody{};

	bool CompleteHeaders();
	template<typename T>
	Task<void> InternalWrite(T s) {
		co_await buffer.Write(s.data(), s.size());
	}
};
