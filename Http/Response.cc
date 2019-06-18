#include "stdafx.h"
#include "Response.h"

namespace {
	std::string const contentLengthKey = "Content-Length";
	std::string const serverKey = "Server";
	char const defaultResponse[] = "HTTP/1.1 200 OK\r\n";
	ptrdiff_t constexpr minimumHeaderBufferSize = 512;

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

Response::Response(TcpSocket& client, char* begin, char* end) :
	begin(begin),
	next(begin),
	end(end),
	outputStreamBuffer(*this, client),
	responseStream(&outputStreamBuffer),
	wroteContentLength(false),
	wroteServer(false),
	inBody(false) {
	if(end - begin < minimumHeaderBufferSize) {
		throw std::runtime_error("minimumHeaderBufferSize");
	}
}

void Response::WriteHeader(xstring const& name, xstring const& value) {
	if(next == begin) {
		WriteStatus(StatusLines::OK);
	}
	auto nameSize = name.second - name.first;
	auto valueSize = value.second - value.first;
	if(nameSize + valueSize + 4 >= end - next) {
		throw std::runtime_error("Response::WriteHeader");
	}
	if(contentLengthKey.size() == static_cast<size_t>(nameSize) && _strcmpi(name.first, contentLengthKey.c_str()) == 0) {
		if(wroteContentLength) {
			throw std::logic_error("wroteContentLength");
		}
		wroteContentLength = true;
	} else if(serverKey.size() == static_cast<size_t>(nameSize) && _strcmpi(name.first, serverKey.c_str()) == 0) {
		if(wroteServer) {
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

void Response::Close() {
	if(!inBody && next != begin) {
		if(!wroteContentLength) {
			static char constexpr zero[] = "0";
			WriteHeader(contentLengthKey, xstring{ zero, zero + sizeof(zero) - 1 });
		}
	}
	outputStreamBuffer.Close();
}

bool Response::CompleteHeaders() {
	// Write the required headers.
	WriteHeader("Date", GetTime());
	if(!wroteServer) {
		WriteHeader("Server", "C++");
	}
	bool isChunked = !wroteContentLength;
	if(isChunked) {
		WriteHeader("Transfer-Encoding", "chunked");
	}

	// Write "end of headers."
	*next++ = '\r';
	*next++ = '\n';
	return isChunked;
}

Response::nstreambuf::nstreambuf(Response& response, TcpSocket& client) :
	response(response),
	client(client),
	closeFn(&nstreambuf::SimpleClose),
	internalSendBufferFn(&nstreambuf::InternalSendBuffer),
	internalSendFn(&nstreambuf::InternalSend),
	sendFn(&nstreambuf::InitialSend),
	begin(response.begin),
	next(response.next),
	end(response.end) {
	static_assert(sizeof(char_type) == sizeof(char));
}

void Response::nstreambuf::Close() {
	(this->*closeFn)();
}

std::streamsize Response::nstreambuf::xsputn(char_type const* s, std::streamsize n) {
	(this->*sendFn)(s, n);
	return n;
}

int Response::nstreambuf::overflow(int c) {
	if(c != traits_type::eof()) {
		char_type ch = traits_type::to_char_type(c);
		xsputn(&ch, 1);
	}
	return c;
}

void Response::nstreambuf::InitialSend(char_type const* s, std::streamsize n) {
	// Have the Response object complete its headers.
	sendFn = &nstreambuf::Send;
	bool isChunked = response.CompleteHeaders();

	// Send the headers.
	begin = response.begin;
	end = response.end;
	InternalSend(begin, response.next - begin);
	next = begin;

	// If this is a chunked response, switch to ChunkedSend and ChunkedClose.
	if(isChunked) {
		closeFn = &nstreambuf::ChunkedClose;
		internalSendBufferFn = &nstreambuf::InternalSendBufferChunk;
		internalSendFn = &nstreambuf::InternalSendChunk;
	}

	// Send the data provided in [s, s + n).
	Send(s, n);
}

void Response::nstreambuf::Send(char_type const* s, std::streamsize n) {
	// If the buffer can hold the provided data, copy it in.  Otherwise, send
	// the buffer.
	if(end - next >= n) {
		memcpy(next, s, n);
		next += n;
	} else {
		(this->*internalSendBufferFn)();

		// If the buffer can hold the provided data, copy it in.  Otherwise,
		// send the data.
		if(end - begin >= n) {
			memcpy(next, s, n);
			next += n;
		} else {
			(this->*internalSendFn)(s, n);
		}
	}
}

void Response::nstreambuf::ChunkedClose() {
	// Send any remaining data as a chunk.
	InternalSendBufferChunk();

	// Send the "last-chunk" and CRLF.
	InternalSend("0\r\n\r\n", 5);
}

void Response::nstreambuf::SimpleClose() {
	// If there are no data in the response, do nothing.
	if(response.next != begin) {
		// If there are data but none has been sent, the headers have not yet
		// been completed.
		if(sendFn == &nstreambuf::InitialSend) {
			response.CompleteHeaders();
			next = response.next;
		}

		// Send any remaining data.
		InternalSendBuffer();
	}
}

void Response::nstreambuf::InternalSendBuffer() {
	InternalSend(begin, next - begin);
	next = begin;
}

void Response::nstreambuf::InternalSend(char_type const* s, std::streamsize n) {
	for(decltype(n) i = 0; i < n;) {
		auto v = client.Send(s + i, n - i);
		if(v > 0) {
			i += v;
		} else {
			throw std::runtime_error("Response::nstreambuf::InternalSend");
		}
	}
}

void Response::nstreambuf::InternalSendBufferChunk() {
	InternalSendChunk(begin, next - begin);
	next = begin;
}

void Response::nstreambuf::InternalSendChunk(char_type const* s, std::streamsize n) {
	if(n > 0) {
		char chunkSize[16];
		unsigned count = snprintf(chunkSize, _countof(chunkSize), "%llx\r\n", n);
		InternalSend(chunkSize, count);
		InternalSend(s, n);
		InternalSend("\r\n", 2);
	}
}
