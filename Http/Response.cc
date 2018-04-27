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
	// Add the overridable headers.
	headers.insert({ "Server", "C++" });

	// Add the non-overridable headers.
	headers["Date"]= GetTime();
	headers["Content-Length"]= std::to_string(text.size());

	// Construct the response.
	auto it= result_codes.find(responseCode);
	auto const* responseCodeText= it == result_codes.cend() ? "Custom Response Code" : it->second;
	std::stringstream ss;
	ss << "HTTP/1.1 " << std::to_string(responseCode) << ' ' << responseCodeText << "\r\n";
	for(auto const& header : headers) {
		ss << header.first << ": " << header.second << "\r\n";
	}
	ss << "\r\n" << text;
	response= ss.str();
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
