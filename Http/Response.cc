#include "stdafx.h"
#include "Response.h"
#include "ResultCodes.inl"

namespace {
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

Response::Response(TcpSocket& client) : outputStreamBuffer(client), responseStream(&outputStreamBuffer) {}

Response::~Response() {}

void Response::Ok(std::string const& text) {
	End(200, text);
}

void Response::End(unsigned short statusCode, std::string const& text/*= empty*/) {
	// Add the overridable headers.
	headers.insert({ "Server", "C++" });

	// Add the non-overridable headers.
	headers["Date"] = GetTime();
	headers["Content-Length"] = std::to_string(text.size());

	// Construct the response.
	auto it = result_codes.find(statusCode);
	auto const* responseCodeText = it == result_codes.cend() ? "Custom Response Code" : it->second;
	responseStream << "HTTP/1.1 " << std::to_string(statusCode) << ' ' << responseCodeText << "\r\n";
	for(auto const& header : headers) {
		responseStream << header.first << ": " << header.second << "\r\n";
	}
	responseStream << "\r\n" << text;
}

Response::nstreambuf::nstreambuf(TcpSocket& client) : client(client) {}

int Response::nstreambuf::sync() {
	Send(true);
	return 0;
}

std::streamsize Response::nstreambuf::xsputn(char_type const* s, std::streamsize n) {
	buffer.insert(buffer.cend(), s, s + n);
	Send();
	return n;
}

int Response::nstreambuf::overflow(int c) {
	if(c != traits_type::eof()) {
		char_type ch = traits_type::to_char_type(c);
		xsputn(&ch, 1);
	}
	return c;
}

void Response::nstreambuf::Send(bool any /*= false*/) {
	if(any || buffer.size() >= 0x800) {
		for(size_t i = 0, n = buffer.size(); i < n;) {
			auto v = client.Send(buffer.data() + i, n - i);
			if(v > 0) {
				i += v;
			} else {
				throw std::runtime_error("Response::nstreambuf::Send");
			}
		}
		buffer.clear();
	}
}
