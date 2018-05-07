#include "stdafx.h"
#include "Response.h"
#include "ResultCodes.inl"

namespace {
	std::string const contentLengthKey = "Content-Length";

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

std::string const Response::empty;

Response::Response(TcpSocket& client) :
	outputStreamBuffer(*this, client),
	responseStream(&outputStreamBuffer),
	statusCode(200) {}

Response::~Response() {}

void Response::Ok(std::string const& text) {
	End(200, text);
}

void Response::End(unsigned short statusCode_, std::string const& text/*= empty*/) {
	statusCode = statusCode_;

	// Add the non-overridable headers.
	headers[contentLengthKey] = std::to_string(text.size());

	// Send the response.
	responseStream << text;
}

void Response::Close() {
	outputStreamBuffer.Close();
}

bool Response::SendHeaders() {
	// Add the overridable headers.
	headers.insert({ "Server", "C++" });

	// Add the non-overridable headers.
	headers["Date"] = GetTime();
	bool isChunked = headers.find(contentLengthKey) == headers.cend();
	if(isChunked) {
		headers["Transfer-Encoding"] = "chunked";
	}

	// Construct and send the response headers.
	auto it = result_codes.find(statusCode);
	auto const* responseCodeText = it == result_codes.cend() ? "Custom Response Code" : it->second;
	responseStream << "HTTP/1.1 " << std::to_string(statusCode) << ' ' << responseCodeText << "\r\n";
	for(auto const& header : headers) {
		responseStream << header.first << ": " << header.second << "\r\n";
	}
	responseStream << "\r\n";
	return isChunked;
}

Response::nstreambuf::nstreambuf(Response& response, TcpSocket& client) :
	response(response),
	client(client),
	sendFn(&nstreambuf::InitialSend),
	closeFn(&nstreambuf::SimpleClose) {}

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

bool Response::nstreambuf::GetIsFull() const {
	return buffer.size() >= 0x800;
}

void Response::nstreambuf::InitialSend(char_type const* s, std::streamsize n) {
	// Have the Response object send its headers.
	sendFn = &nstreambuf::SimpleSend;
	bool isChunked = response.SendHeaders();

	// Flush the response so far.
	InternalSend();

	// If this is a chunked response, switch to ChunkedSend and ChunkedClose.
	if(isChunked) {
		sendFn = &nstreambuf::ChunkedSend;
		closeFn = &nstreambuf::ChunkedClose;
	}

	// Send the data provided in [s, s + n).
	(this->*sendFn)(s, n);
}

void Response::nstreambuf::ChunkedSend(char_type const* s, std::streamsize n) {
	// Add the data provided in [s, s + n) to the buffer.
	buffer.insert(buffer.cend(), s, s + n);

	// If the buffer is large enough, send it as a chunk.
	if(IsFull) {
		InternalSendChunk();
	}
}

void Response::nstreambuf::SimpleSend(char_type const* s, std::streamsize n) {
	// Add the data provided in [s, s + n) to the buffer.
	buffer.insert(buffer.cend(), s, s + n);

	// If the buffer is large enough, send it.
	if(IsFull) {
		InternalSend();
	}
}

void Response::nstreambuf::ChunkedClose() {
	// Send any remaining data as a chunk.
	InternalSendChunk();

	// Send the "last-chunk" and CRLF.
	InternalSend("0\r\n\r\n", 5);
}

void Response::nstreambuf::SimpleClose() {
	// Send any remaining data.
	InternalSend();
}

void Response::nstreambuf::InternalSend() {
	InternalSend(buffer.data(), buffer.size());
	buffer.clear();
}

void Response::nstreambuf::InternalSend(char const* s, size_t n) {
	for(size_t i = 0; i < n;) {
		auto v = client.Send(s + i, n - i);
		if(v > 0) {
			i += v;
		} else {
			throw std::runtime_error("Response::nstreambuf::InternalSend");
		}
	}
}

void Response::nstreambuf::InternalSendChunk() {
	unsigned n = static_cast<unsigned>(buffer.size());
	if(n > 0) {
		char chunkSize[16];
		unsigned count = snprintf(chunkSize, _countof(chunkSize), "%x\r\n", n);
		InternalSend(chunkSize, count);
		InternalSend();
		InternalSend("\r\n", 2);
	}
}
