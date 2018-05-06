#pragma once

#include "HeaderMap.h"
#include "TcpSocket.h"

class Response {
public:
	Response(TcpSocket& client);
	~Response();
	std::ostream& GetResponseStream() { return responseStream; }
	HeaderMap& GetHeaders() { return headers; }
	void Ok(std::string const& text);
	void End(unsigned short statusCode, std::string const& text = empty);
	__declspec(property(get = GetResponseStream)) std::ostream& ResponseStream;
	__declspec(property(get = GetHeaders)) HeaderMap& Headers;

private:
	struct nstreambuf : public std::streambuf {
		nstreambuf(TcpSocket& client);
		int sync() override;
		std::streamsize xsputn(char_type const* s, std::streamsize n) override;
		int overflow(int c) override;

	private:
		TcpSocket& client;
		std::vector<nstreambuf::char_type> buffer;

		void Send(bool any = false);
	};

	nstreambuf outputStreamBuffer;
	std::ostream responseStream;
	HeaderMap headers;
	static std::string const empty;
	std::string response;
};
