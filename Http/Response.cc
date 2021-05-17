#include "pch.h"
#include "Response.h"

std::array<Response::nstreambuf::StateFunctions, 5> Response::nstreambuf::states = {
	Response::nstreambuf::StateFunctions{
		&Response::nstreambuf::InitialWriteStatusLine,
		&Response::nstreambuf::InitialWriteHeader,
		&Response::nstreambuf::InitialWriteBody,
		&Response::nstreambuf::InitialWrite,
		&Response::nstreambuf::InitialClose,
	},
	Response::nstreambuf::StateFunctions{
		&Response::nstreambuf::InHeadersWriteStatusLine,
		&Response::nstreambuf::InHeadersWriteHeader,
		&Response::nstreambuf::InHeadersWriteBody,
		&Response::nstreambuf::InHeadersWrite,
		&Response::nstreambuf::InHeadersClose,
	},
	Response::nstreambuf::StateFunctions{
		&Response::nstreambuf::SentSomeHeadersWriteStatusLine,
		&Response::nstreambuf::SentSomeHeadersWriteHeader,
		&Response::nstreambuf::SentSomeHeadersWriteBody,
		&Response::nstreambuf::SentSomeHeadersWrite,
		&Response::nstreambuf::SentSomeHeadersClose,
	},
	Response::nstreambuf::StateFunctions{
		&Response::nstreambuf::InChunkedBodyWriteStatusLine,
		&Response::nstreambuf::InChunkedBodyWriteHeader,
		&Response::nstreambuf::InChunkedBodyWriteBody,
		&Response::nstreambuf::InChunkedBodyWrite,
		&Response::nstreambuf::InChunkedBodyClose,
	},
	Response::nstreambuf::StateFunctions{
		&Response::nstreambuf::InBodyWriteStatusLine,
		&Response::nstreambuf::InBodyWriteHeader,
		&Response::nstreambuf::InBodyWriteBody,
		&Response::nstreambuf::InBodyWrite,
		&Response::nstreambuf::InBodyClose,
	},
};

namespace {
	std::streamsize const chunkedLengthSize = 6;
	std::string const contentLengthKey = "Content-Length";
	std::string const dateKey = "Date";
	std::string const serverKey = "Server";
	std::string const transferEncodingKey = "Transfer-Encoding";
	enum { initialStateIndex, inHeadersStateIndex, sentSomeHeadersStateIndex, inChunkedBodyStateIndex, inBodyStateIndex };

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

	void LogicError(char const* message) {
#ifdef _DEBUG
		throw std::logic_error(message);
#else
		std::cerr << message << std::endl;
#endif
	}

	inline char AsHexDigit(std::streamsize value) noexcept {
		value &= 0x0f;
		return static_cast<char>(value < 10 ? value + '0' : value - 10 + 'A');
	}

	void SetLength(char* p, std::streamsize value) {
		p += 4;
		p[0] = '\r';
		p[1] = '\n';
		for (auto i = 0; i < 4; ++i) {
			*--p = AsHexDigit(value);
			value >>= 4;
		}
	}
}

Response::Response(TcpSocket& socket, char* begin, char* end) : outputStreamBuffer(socket, begin, end), responseStream(&outputStreamBuffer) {
	if (end - begin < MinimumBufferSize) {
		throw std::runtime_error("MinimumBufferSize");
	}
}

Response::Response(Response&& that) noexcept : outputStreamBuffer(std::move(that.outputStreamBuffer)), responseStream(&outputStreamBuffer) {}

Response::nstreambuf::nstreambuf(TcpSocket& socket, char* begin, char* end) :
	socket(socket),
	begin(begin),
	end(end) {
	static_assert(sizeof(char_type) == sizeof(char));
}

Response::nstreambuf::nstreambuf(nstreambuf&& that) noexcept : socket(that.socket) {
	std::swap(begin, that.begin);
	std::swap(next, that.next);
	std::swap(end, that.end);
	std::swap(contentLength, that.contentLength);
	std::swap(stateIndex, that.stateIndex);
	std::swap(wroteServer, that.wroteServer);
}

void Response::nstreambuf::WriteStatusLine(std::string const& statusLine) {
	(this->*states[stateIndex].WriteStatusLine)(statusLine);
}

void Response::nstreambuf::WriteHeader(xstring const& name, xstring const& value) {
	(this->*states[stateIndex].WriteHeader)(name, value);
}

void Response::nstreambuf::Close() {
	(this->*states[stateIndex].Close)();
}

bool Response::nstreambuf::Reset() {
	if (stateIndex == initialStateIndex || stateIndex == inHeadersStateIndex) {
		InternalReset();
		return true;
	}
	return false;
}

std::streamsize Response::nstreambuf::xsputn(char_type const* s, std::streamsize n) {
	WriteBody(s, n);
	return n;
}

int Response::nstreambuf::overflow(int c) {
	if (c != traits_type::eof()) {
		char_type const ch = traits_type::to_char_type(c);
		xsputn(&ch, 1);
	}
	return c;
}

bool Response::nstreambuf::get_IsResettable() const {
	return stateIndex == initialStateIndex;
}

void Response::nstreambuf::WriteBody(char_type const* s, std::streamsize n) {
	(this->*states[stateIndex].WriteBody)(s, n);
}

void Response::nstreambuf::Write(char_type const* s, std::streamsize n) {
	(this->*states[stateIndex].Write)(s, n);
}

void Response::nstreambuf::CheckWriteHeader(xstring const& name, xstring const& value) {
	size_t nameSize = name.second - name.first;
	if (contentLengthKey.size() == static_cast<size_t>(nameSize) && _strnicmp(name.first, contentLengthKey.c_str(), nameSize) == 0) {
		if (contentLength) {
			return LogicError("cannot write Content-Length header more than once");
		}
		contentLength = std::strtoull(value.first, nullptr, 10);
	} else if (serverKey.size() == static_cast<size_t>(nameSize) && _strnicmp(name.first, serverKey.c_str(), nameSize) == 0) {
		if (wroteServer) {
			return LogicError("cannot write Server header more than once");
		}
		wroteServer = true;
	} else if (transferEncodingKey.size() == static_cast<size_t>(nameSize) && _strnicmp(name.first, transferEncodingKey.c_str(), nameSize) == 0) {
		return LogicError("cannot write Transfer-Encoding header");
	} else if (dateKey.size() == static_cast<size_t>(nameSize) && _strnicmp(name.first, dateKey.c_str(), nameSize) == 0) {
		return LogicError("cannot write Date header");
	}
}

void Response::nstreambuf::InternalWriteHeader(xstring const& name, xstring const& value) {
	Write(name.first, name.second - name.first);
	Write(": ", 2);
	Write(value.first, value.second - value.first);
	Write("\r\n", 2);
}

void Response::nstreambuf::WriteServerHeaders() {
	auto const time = GetTime();
	InternalWriteHeader({ dateKey.data(), dateKey.data() + dateKey.size() }, { time.data(), time.data() + time.size() }); // TODO:  replace std::string key values with xstring key values.
	if (!wroteServer) {
		static char_type const p[] = "C++";
		InternalWriteHeader({ serverKey.data(), serverKey.data() + serverKey.size() }, { p, p + sizeof(p) - 1 });
	}
}

void Response::nstreambuf::SendBuffer() {
	socket.Send(begin, next - begin);
	next = begin;
}

void Response::nstreambuf::InternalReset() {
	stateIndex = initialStateIndex;
	next = begin;
}

void Response::nstreambuf::InitialWriteStatusLine(std::string const& statusLine) {
	statusLine.copy(begin, statusLine.size());
	next = begin + statusLine.size();
}

void Response::nstreambuf::InitialWriteHeader(xstring const& name, xstring const& value) {
	WriteStatusLine(StatusLines::OK);
	stateIndex = inHeadersStateIndex;
	WriteHeader(name, value);
}

void Response::nstreambuf::InitialWriteBody(char_type const* s, std::streamsize n) {
	WriteStatusLine(StatusLines::OK);
	stateIndex = inHeadersStateIndex;
	WriteBody(s, n);
}

void Response::nstreambuf::InitialWrite(char_type const* /*s*/, std::streamsize /*n*/) {
	assert(false);
}

void Response::nstreambuf::InitialClose() {
	WriteStatusLine(StatusLines::NoContent);
	stateIndex = inHeadersStateIndex;
	Close();
}

void Response::nstreambuf::InHeadersWriteStatusLine(std::string const& /*statusLine*/) {
	LogicError("cannot write status line after headers");
}

void Response::nstreambuf::InHeadersWriteHeader(xstring const& name, xstring const& value) {
	CheckWriteHeader(name, value);
	InternalWriteHeader(name, value);
}

void Response::nstreambuf::InHeadersWriteBody(char_type const* s, std::streamsize n) {
	WriteServerHeaders();
	if (contentLength) {
		// End the headers.
		Write("\r\n", 2);
		stateIndex = inBodyStateIndex;
	} else {
		// There is no content length header.  Use "chunked" transfer.
		static char_type const p[] = "chunked";
		InternalWriteHeader({ transferEncodingKey.data(), transferEncodingKey.data() + transferEncodingKey.size() }, { p, p + sizeof(p) - 1 });

		// End the headers, send the buffer, and adjust it for the chunk length.
		Write("\r\n", 2);
		SendBuffer();
		next += chunkedLengthSize;
		stateIndex = inChunkedBodyStateIndex;
	}
	WriteBody(s, n);
}

void Response::nstreambuf::InHeadersWrite(char_type const* s, std::streamsize n) {
	auto navailable = end - next;
	if (navailable < n) {
		// Copy as much data into the buffer as possible.
		memcpy_s(next, navailable, s, navailable);
		next += navailable;

		// Send the buffer.
		SendBuffer();

		// Set the state to "sent some headers".
		stateIndex = sentSomeHeadersStateIndex;

		// Write the remaining data.
		Write(s + navailable, n - navailable);
	} else {
		// Copy all data into the buffer.
		memcpy_s(next, navailable, s, n);
		next += n;
	}
}

void Response::nstreambuf::InHeadersClose() {
	WriteServerHeaders();
	Write("\r\n", 2);
	SendBuffer();
	stateIndex = initialStateIndex;
}

void Response::nstreambuf::SentSomeHeadersWriteStatusLine(std::string const& /*statusLine*/) {
	return LogicError("cannot write status line after some headers sent");
}

void Response::nstreambuf::SentSomeHeadersWriteHeader(xstring const& name, xstring const& value) {
	InHeadersWriteHeader(name, value);
}

void Response::nstreambuf::SentSomeHeadersWriteBody(char_type const* s, std::streamsize n) {
	InHeadersWriteBody(s, n);
}

void Response::nstreambuf::SentSomeHeadersWrite(char_type const* s, std::streamsize n) {
	for (;;) {
		auto navailable = end - next;
		if (navailable < n) {
			// Copy as much data into the buffer as possible.
			memcpy_s(next, navailable, s, navailable);
			next += navailable;

			// Send the buffer.
			SendBuffer();

			// Adjust the parameters.
			s += navailable;
			n -= navailable;
		} else {
			// Copy all data into the buffer.
			memcpy_s(next, navailable, s, n);
			next += n;
			break;
		}
	}
}

void Response::nstreambuf::SentSomeHeadersClose() {
	InHeadersClose();
}

void Response::nstreambuf::InChunkedBodyWriteStatusLine(std::string const& /*statusLine*/) {
	return LogicError("cannot write status line after some body sent");
}

void Response::nstreambuf::InChunkedBodyWriteHeader(xstring const& /*name*/, xstring const& /*value*/) {
	return LogicError("cannot write headers after some body sent");
}

void Response::nstreambuf::InChunkedBodyWriteBody(char_type const* s, std::streamsize n) {
	Write(s, n);
}

void Response::nstreambuf::InChunkedBodyWrite(char_type const* s, std::streamsize n) {
	for (;;) {
		// Chech availablility, accounting for CRLF.
		auto navailable = end - next - 2;
		if (navailable < n) {
			// Copy as much data into the buffer as possible.
			memcpy_s(next, navailable, s, navailable);

			// Set the length in the buffer.
			SetLength(begin, end - begin - chunkedLengthSize - 2);

			// Add CRLF to the buffer.
			end[-2] = '\r';
			end[-1] = '\n';

			// Send the buffer.
			next = end;
			SendBuffer();
			next += chunkedLengthSize;

			// Adjust the parameters.
			s += navailable;
			n -= navailable;
		} else {
			// Copy all data into the buffer.
			memcpy_s(next, navailable, s, n);
			next += n;
			break;
		}
	}
}

void Response::nstreambuf::InChunkedBodyClose() {
	if (begin + chunkedLengthSize < next) {
		// Set the length in the buffer.
		SetLength(begin, next - begin - chunkedLengthSize);

		// Add CRLF to the buffer.
		next[0] = '\r';
		next[1] = '\n';
		next += 2;

		// Send the buffer if necessary.
		if (end - next < 5) {
			SendBuffer();
		}
	} else {
		next = begin;
	}

	// Add 0 CRLF CRLF to buffer.
	memcpy_s(next, 5, "0\r\n\r\n", 5);
	next += 5;

	// Send the buffer.
	SendBuffer();
	stateIndex = initialStateIndex;
}

void Response::nstreambuf::InBodyWriteStatusLine(std::string const& /*statusLine*/) {
	return LogicError("cannot write status line after some body sent");
}

void Response::nstreambuf::InBodyWriteHeader(xstring const& /*name*/, xstring const& /*value*/) {
	return LogicError("cannot write headers after some body sent");
}

void Response::nstreambuf::InBodyWriteBody(char_type const* s, std::streamsize n) {
	Write(s, n);
}

void Response::nstreambuf::InBodyWrite(char_type const* s, std::streamsize n) {
	SentSomeHeadersWrite(s, n);
}

void Response::nstreambuf::InBodyClose() {
	SendBuffer();
	stateIndex = initialStateIndex;
}
