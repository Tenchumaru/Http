#pragma once

#include "xtypes.h"
#include "StatusLines.h"
#include "AsyncSocket.h"

class AsyncResponse {
public:
	AsyncResponse(AsyncSocket& socket, char* begin, char* end);
	AsyncResponse() = delete;
	AsyncResponse(AsyncResponse const&) = delete;
	AsyncResponse(AsyncResponse&&) = default;
	AsyncResponse& operator=(AsyncResponse const&) = delete;
	AsyncResponse& operator=(AsyncResponse&&) = default;
	template<typename T>
	Task<void> Write(T t) {
		inBody = true;
		std::stringstream ss;
		ss << t;
		auto s = ss.str();
		return Write(s);
	}
	template<>
	Task<void> Write(std::string const& s) {
		return InternalWrite(s);
	}
	void WriteStatus(StatusLines::StatusLine const& statusLine) {
		if (next != begin) {
			throw std::logic_error("WriteStatus");
		}
		memcpy(begin, statusLine.first, statusLine.second);
		next = begin + statusLine.second;
	}
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
	struct Buffer {
		Buffer(AsyncResponse& response, AsyncSocket& socket);
		Task<void> Close();
		Task<void> Write(void const* s, size_t n);

	private:
		using Fn0 = Task<void>(Buffer::*)();
		using Fn2 = Task<void>(Buffer::*)(void const* s, size_t n);

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

	char* begin;
	char* next;
	char* end;
	Buffer outputStreamBuffer;
	bool wroteContentLength;
	bool wroteServer;
	bool inBody;

	bool CompleteHeaders();
	Task<void> InternalWrite(std::string const& s);
};
