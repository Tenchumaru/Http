#include "pch.h"
#include "AsyncResponse.h"

namespace {
	std::string const contentLengthKey = "Content-Length";
	std::string const serverKey = "Server";
	char const defaultResponse[] = "HTTP/1.1 200 OK\r\n";
	constexpr ptrdiff_t minimumHeaderBufferSize = 512;

	// TODO:  consider creating a thread that runs once per second to update
	// the time buffer.
	std::string GetTime() {
		time_t t = time(nullptr);
#ifdef _WIN32
		tm m_;
		gmtime_s(&m_, &t);
		tm* m = &m_;
#else
		tm* m = gmtime(&t);
#endif
		char buf[88];
		strftime(buf, sizeof(buf), "%a, %d %b %Y %T GMT", m);
		return buf;
	}
}

AsyncResponse::AsyncResponse(AsyncSocket& socket, char* begin, char* end) :
	begin(begin),
	next(begin),
	end(end),
	outputStreamBuffer(*this, socket),
	wroteContentLength(false),
	wroteServer(false),
	inBody(false) {
	if (end - begin < minimumHeaderBufferSize) {
		throw std::runtime_error("minimumHeaderBufferSize");
	}
}

void AsyncResponse::WriteHeader(xstring const& name, xstring const& value) {
	if (next == begin) {
		WriteStatus(StatusLines::OK);
	}
	auto nameSize = name.second - name.first;
	auto valueSize = value.second - value.first;
	if (nameSize + valueSize + 4 >= end - next) {
		throw std::runtime_error("AsyncResponse::WriteHeader");
	}
	if (contentLengthKey.size() == static_cast<size_t>(nameSize) && _strcmpi(name.first, contentLengthKey.c_str()) == 0) {
		if (wroteContentLength) {
			throw std::logic_error("wroteContentLength");
		}
		wroteContentLength = true;
	} else if (serverKey.size() == static_cast<size_t>(nameSize) && _strcmpi(name.first, serverKey.c_str()) == 0) {
		if (wroteServer) {
			throw std::logic_error("wroteServer");
		}
		wroteServer = true;
	}
	next = std::copy(name.first, name.second, next);
	*next++ = ':';
	*next++ = ' ';
	next = std::copy(value.first, value.second, next);
	*next++ = '\r';
	*next++ = '\n';
}

Task<void> AsyncResponse::Close() {
	if (!inBody && next != begin) {
		if (!wroteContentLength) {
			static constexpr char zero[] = "0";
			WriteHeader(contentLengthKey, xstring{ zero, zero + sizeof(zero) - 1 });
		}
	}
	return outputStreamBuffer.Close();
}

bool AsyncResponse::CompleteHeaders() {
	// Write the required headers.
	WriteHeader("Date", GetTime());
	if (!wroteServer) {
		WriteHeader("Server", "C++");
	}
	bool isChunked = !wroteContentLength;
	if (isChunked) {
		WriteHeader("Transfer-Encoding", "chunked");
	}

	// Write "end of headers."
	*next++ = '\r';
	*next++ = '\n';
	return isChunked;
}

AsyncResponse::Buffer::Buffer(AsyncResponse& response, AsyncSocket& socket) :
	response(response),
	socket(socket),
	closeFn(&Buffer::SimpleClose),
	internalSendBufferFn(&Buffer::InternalSendBuffer),
	internalSendFn(&Buffer::InternalSend),
	sendFn(&Buffer::InitialSend),
	begin(response.begin),
	next(response.next),
	end(response.end) {}

Task<void> AsyncResponse::Buffer::Close() {
	return (this->*closeFn)();
}

Task<void> AsyncResponse::Buffer::Write(void const* s, size_t n) {
	return (this->*sendFn)(s, n);
}

Task<void> AsyncResponse::Buffer::InitialSend(void const* s, size_t n) {
	// Have the AsyncResponse object complete its headers.
	sendFn = &Buffer::Send;
	bool isChunked = response.CompleteHeaders();

	// Send the headers.
	begin = response.begin;
	end = response.end;
	co_await InternalSend(begin, response.next - begin);
	next = begin;

	// If this is a chunked response, switch to ChunkedSend and ChunkedClose.
	if (isChunked) {
		closeFn = &Buffer::ChunkedClose;
		internalSendBufferFn = &Buffer::InternalSendBufferChunk;
		internalSendFn = &Buffer::InternalSendChunk;
	}

	// Send the data provided in [s, s + n).
	co_await Send(s, n);
}

Task<void> AsyncResponse::Buffer::Send(void const* s, size_t n) {
	// If the buffer can hold the provided data, copy it in.  Otherwise, send
	// the buffer.
	auto d = static_cast<ptrdiff_t>(n);
	if (end - next >= d) {
		memcpy(next, s, n);
		next += n;
	} else {
		co_await(this->*internalSendBufferFn)();

		// If the buffer can hold the provided data, copy it in.  Otherwise,
		// send the data.
		if (end - begin >= d) {
			memcpy(next, s, n);
			next += n;
		} else {
			co_await(this->*internalSendFn)(s, n);
		}
	}
}

Task<void> AsyncResponse::Buffer::ChunkedClose() {
	// Send any remaining data as a chunk.
	co_await InternalSendBufferChunk();

	// Send the "last-chunk" and CRLF.
	co_await InternalSend("0\r\n\r\n", 5);
}

Task<void> AsyncResponse::Buffer::SimpleClose() {
	// If there are no data in the response, do nothing.
	if (response.next != begin) {
		// If there are data but none has been sent, the headers have not yet
		// been completed.
		if (sendFn == &Buffer::InitialSend) {
			response.CompleteHeaders();
			next = response.next;
		}

		// Send any remaining data.
		co_await InternalSendBuffer();
	}
}

Task<void> AsyncResponse::Buffer::InternalSendBuffer() {
	co_await InternalSend(begin, next - begin);
	next = begin;
}

Task<void> AsyncResponse::Buffer::InternalSend(void const* s, size_t n) {
	auto const* p = static_cast<std::uint8_t const*>(s);
	for (decltype(n) i = 0; i < n;) {
		auto [j, errorCode] = co_await socket.Send(p + i, n - i);
		if (errorCode) {
			throw std::runtime_error("AsyncResponse::Buffer::InternalSend");
		}
		i += j;
	}
}

Task<void> AsyncResponse::Buffer::InternalSendBufferChunk() {
	co_await InternalSendChunk(begin, next - begin);
	next = begin;
}

Task<void> AsyncResponse::Buffer::InternalSendChunk(void const* s, size_t n) {
	if (n > 0) {
		char chunkSize[16];
		unsigned count = snprintf(chunkSize, _countof(chunkSize), "%llx\r\n", n);
		co_await InternalSend(chunkSize, count);
		co_await InternalSend(s, n);
		co_await InternalSend("\r\n", 2);
	}
}
