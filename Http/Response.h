#pragma once

#include "xtypes.h"
#include "StatusLines.h"
#include "TcpSocket.h"

class Response {
public:
	constexpr static ptrdiff_t MinimumHeaderBufferSize = 512;

	// TODO:  consider creating a flushable response to allow for
	// content larger than the provided buffer.
	Response(TcpSocket& socket, char* begin, char* end);
	Response() = delete;
	Response(Response const&) = delete;
	Response(Response&& that) noexcept;
	Response& operator=(Response const&) = delete;
	Response& operator=(Response&&) noexcept = delete;
	void WriteStatus(StatusLines::StatusLine const& statusLine);
	void WriteHeader(std::string const& name, std::string const& value) {
		WriteHeader(xstring{ name.data(), name.data() + name.size() }, xstring{ value.data(), value.data() + value.size() });
	}
	void WriteHeader(std::string const& name, xstring const& value) {
		WriteHeader(xstring{ name.data(), name.data() + name.size() }, value);
	}
	void WriteHeader(xstring const& name, xstring const& value);
	bool Reset();
	template<typename T>
	Response& operator<<(T t) {
		inBody = true;
		responseStream << t;
		return *this;
	}

protected:
	// These are protected since derived classes control the response lifetime.
	~Response() = default;
	void Close();

private:
	struct nstreambuf : public std::streambuf {
		nstreambuf(Response& response, TcpSocket& socket);
		nstreambuf() = delete;
		nstreambuf(nstreambuf const&) = delete;
		nstreambuf(nstreambuf&& that) noexcept;
		nstreambuf& operator=(nstreambuf const&) = delete;
		nstreambuf& operator=(nstreambuf&&) noexcept = delete;
		void Close();
		bool get_HasWritten() const { return hasWritten; }
		__declspec(property(get = get_HasWritten)) bool const HasWritten;
		std::streamsize xsputn(char_type const* s, std::streamsize n) override;
		int overflow(int c) override;

	private:
		using Fn0 = void(nstreambuf::*)();
		using Fn2 = void(nstreambuf::*)(char_type const* s, std::streamsize n);

		Response& response;
		TcpSocket& socket;
		Fn0 closeFn;
		Fn0 internalSendBufferFn;
		Fn2 internalSendFn;
		Fn2 sendFn;
		char* begin;
		char* next;
		char* end;
		bool hasWritten{};

		void InitialSend(char_type const* s, std::streamsize n);
		void Send(char_type const* s, std::streamsize n);
		void ChunkedClose();
		void SimpleClose();
		void InternalSendBuffer();
		void InternalSend(char_type const* s, std::streamsize n);
		void InternalSendBufferChunk();
		void InternalSendChunk(char_type const* s, std::streamsize n);
	};

	char* begin{};
	char* next{};
	char* end{};
	nstreambuf outputStreamBuffer;
	std::ostream responseStream;
	bool wroteContentLength{};
	bool wroteServer{};
	bool inBody{};

	bool CompleteHeaders();
};
