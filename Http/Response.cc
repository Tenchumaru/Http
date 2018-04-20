#include "stdafx.h"
#include "Response.h"
#include "ResultCodes.inl"

namespace {
	std::string GetTime() {
		time_t t= time(nullptr);
#ifdef _WIN32
		tm m_;
		gmtime_s(&m_, &t);
		tm* m= &m_;
#else
		tm* m= gmtime(&t);
#endif
		char buf[88];
		strftime(buf, sizeof(buf), "%a, %d %b %Y %T GMT", m);
		return buf;
	}
}

std::string const Response::empty;

Response::Response() {}

Response::~Response() {}

void Response::Ok(std::string const& text) {
	End(200, text);
}

void Response::End(unsigned short responseCode, std::string const& text/*= empty*/) {
	auto it= result_codes.find(responseCode);
	auto const* responseText= it == result_codes.cend() ? "Custom Response Code" : it->second;
	auto responseCodeString= std::to_string(responseCode);
	auto contentLengthString= std::to_string(text.size());
	response= "HTTP/1.1 " + responseCodeString + ' ' + responseText + "\r\n"
		"Server: C++\r\n"
		"Date: " + GetTime() + "\r\n"
		"Content-Length: " + contentLengthString + "\r\n"
		"\r\n" + text;
}

void Response::Send(TcpSocket& client) {
	for(size_t i= 0, n= response.size(); i < n;) {
		auto v= client.Send(response.c_str() + i, n - i);
		if(v > 0) {
			i += v;
		} else {
			break;
		}
	}
}
