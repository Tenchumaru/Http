#include "pch.h"
#include "Response.h"

namespace {
	std::string const contentLengthKey = "Content-Length";
	std::string const serverKey = "Server";

	// TODO:  consider creating a thread that runs once per second to update
	// the time buffer.
	std::string GetTime() {
		time_t const t = time(nullptr);
#ifdef _WIN32
		tm m_;
		gmtime_s(&m_, &t);
		tm* const m = &m_;
#else
		tm* const m = gmtime(&t);
#endif
		char buf[88];
		strftime(buf, sizeof(buf), "%a, %d %b %Y %T GMT", m);
		return buf;
	}
}

Response::Response(TcpSocket& socket, char* begin, char* end) :
	begin(begin),
	next(begin),
	end(end),
	outputStreamBuffer(*this, socket),
	responseStream(&outputStreamBuffer) {
	if (end - begin < MinimumHeaderBufferSize) {
		throw std::runtime_error("MinimumHeaderBufferSize");
	}
}

Response::Response(Response&& that) noexcept : outputStreamBuffer(std::move(that.outputStreamBuffer)), responseStream(&outputStreamBuffer) {
	std::swap(begin, that.begin);
	std::swap(next, that.next);
	std::swap(end, that.end);
	std::swap(wroteContentLength, that.wroteContentLength);
	std::swap(wroteServer, that.wroteServer);
	std::swap(inBody, that.inBody);
}

void Response::WriteStatus(StatusLines::StatusLine const& statusLine) {
	if (next != begin) {
		throw std::logic_error("cannot write Status line more than once");
	}
	memcpy(begin, statusLine.first, statusLine.second);
	next = begin + statusLine.second;
}

void Response::WriteHeader(xstring const& name, xstring const& value) {
	if (next == begin) {
		WriteStatus(StatusLines::OK);
	}
	auto const nameSize = name.second - name.first;
	auto const valueSize = value.second - value.first;
	if (nameSize + valueSize + 4 >= end - next) {
		throw std::runtime_error("Response::WriteHeader");
	}
	if (contentLengthKey.size() == static_cast<size_t>(nameSize) && _strcmpi(name.first, contentLengthKey.c_str()) == 0) {
		if (wroteContentLength) {
			throw std::logic_error("cannot write Content-Length header more than once");
		}
		wroteContentLength = true;
	} else if (serverKey.size() == static_cast<size_t>(nameSize) && _strcmpi(name.first, serverKey.c_str()) == 0) {
		if (wroteServer) {
			throw std::logic_error("cannot write Server header more than once");
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

bool Response::Reset() {
	if (outputStreamBuffer.HasWritten) {
		return false;
	}
	next = begin;
	wroteContentLength = false;
	wroteServer = false;
	inBody = false;
	return true;
}

void Response::Close() {
	if (!inBody && next != begin && !wroteContentLength) {
		static constexpr char zero = '0';
		WriteHeader(contentLengthKey, xstring{ &zero, &zero + sizeof(zero) });
	}
	outputStreamBuffer.Close();
	Reset();
}

bool Response::CompleteHeaders() {
	// Write the required headers.
	WriteHeader("Date", GetTime());
	if (!wroteServer) {
		WriteHeader("Server", "C++");
	}
	bool const isChunked = !wroteContentLength;
	if (isChunked) {
		WriteHeader("Transfer-Encoding", "chunked");
	}

	// Write "end of headers."
	*next++ = '\r';
	*next++ = '\n';
	return isChunked;
}

Response::nstreambuf::nstreambuf(Response& response, TcpSocket& socket) :
	response(response),
	socket(socket),
	closeFn(&nstreambuf::SimpleClose),
	internalSendBufferFn(&nstreambuf::InternalSendBuffer),
	internalSendFn(&nstreambuf::InternalSend),
	sendFn(&nstreambuf::InitialSend),
	begin(response.begin),
	next(response.next),
	end(response.end) {
	static_assert(sizeof(char_type) == sizeof(char));
}

Response::nstreambuf::nstreambuf(nstreambuf&& that) noexcept : response(that.response), socket(that.socket) {
	std::swap(closeFn, that.closeFn);
	std::swap(internalSendBufferFn, that.internalSendBufferFn);
	std::swap(internalSendFn, that.internalSendFn);
	std::swap(sendFn, that.sendFn);
	std::swap(begin, that.begin);
	std::swap(next, that.next);
	std::swap(end, that.end);
	std::swap(hasWritten, that.hasWritten);
}

void Response::nstreambuf::Close() {
	(this->*closeFn)();
	closeFn = &nstreambuf::SimpleClose;
	internalSendBufferFn = &nstreambuf::InternalSendBuffer;
	internalSendFn = &nstreambuf::InternalSend;
	sendFn = &nstreambuf::InitialSend;
	hasWritten = false;
}

std::streamsize Response::nstreambuf::xsputn(char_type const* s, std::streamsize n) {
	(this->*sendFn)(s, n);
	return n;
}

int Response::nstreambuf::overflow(int c) {
	if (c != traits_type::eof()) {
		char_type const ch = traits_type::to_char_type(c);
		xsputn(&ch, 1);
	}
	return c;
}

void Response::nstreambuf::InitialSend(char_type const* s, std::streamsize n) {
	// Have the Response object complete its headers.
	sendFn = &nstreambuf::Send;
	bool const isChunked = response.CompleteHeaders();

	// Send the headers.
	begin = response.begin;
	end = response.end;
	InternalSend(begin, response.next - begin);
	next = begin;

	// If this is a chunked response, switch to ChunkedSend and ChunkedClose.
	if (isChunked) {
		closeFn = &nstreambuf::ChunkedClose;
		internalSendBufferFn = &nstreambuf::InternalSendBufferChunk;
		internalSendFn = &nstreambuf::InternalSendChunk;
	}

	// Send the data provided in [s, s + n).
	Send(s, n);
	hasWritten = true;
}

void Response::nstreambuf::Send(char_type const* s, std::streamsize n) {
	// If the buffer can hold the provided data, copy it in.  Otherwise, send
	// the buffer.
	if (end - next >= n) {
		memcpy(next, s, n);
		next += n;
	} else {
		(this->*internalSendBufferFn)();

		// If the buffer can hold the provided data, copy it in.  Otherwise,
		// send the data.
		if (end - begin >= n) {
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
	if (response.next != begin) {
		// If there are data but none has been sent, the headers have not yet
		// been completed.
		if (sendFn == &nstreambuf::InitialSend) {
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
	for (decltype(n) i = 0; i < n;) {
		auto [v, errorCode] = socket.Send(s + i, n - i);
		if (v > 0) {
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
	if (n > 0) {
		char chunkSize[16];
		unsigned const count = snprintf(chunkSize, _countof(chunkSize), "%llx\r\n", n);
		InternalSend(chunkSize, count);
		InternalSend(s, n);
		InternalSend("\r\n", 2);
	}
}
