#pragma once

#include "HeaderMap.h"
#include "TcpSocket.h"

class Response {
public:
	Response(TcpSocket& client);
	std::ostream& GetResponseStream() { return responseStream; }
	HeaderMap& GetHeaders() { return headers; }
	unsigned short GetStatusCode() const { return statusCode; }
	void SetStatusCode(unsigned short statusCode_) { statusCode = statusCode_; }
	void Ok(std::string const& text);
	void End(unsigned short statusCode, std::string const& text = empty);
	__declspec(property(get = GetResponseStream)) std::ostream& ResponseStream;
	__declspec(property(get = GetHeaders)) HeaderMap& Headers;
	__declspec(property(get = GetStatusCode, put = SetStatusCode)) unsigned short StatusCode;

protected:
	~Response();
	void Close();

private:
	struct nstreambuf : public std::streambuf {
		nstreambuf(Response& response, TcpSocket& client);
		void Close();
		std::streamsize xsputn(char_type const* s, std::streamsize n) override;
		int overflow(int c) override;

	private:
		using SendFn = void(nstreambuf::*)(char_type const* s, std::streamsize n);
		using CloseFn = void(nstreambuf::*)();

		Response& response;
		TcpSocket& client;
		std::vector<nstreambuf::char_type> buffer;
		SendFn sendFn;
		CloseFn closeFn;

		bool GetIsFull() const;
		void InitialSend(char_type const* s, std::streamsize n);
		void ChunkedSend(char_type const* s, std::streamsize n);
		void SimpleSend(char_type const* s, std::streamsize n);
		void ChunkedClose();
		void SimpleClose();
		void InternalSend();
		void InternalSend(char const* s, size_t n);
		void InternalSendChunk();
		__declspec(property(get = GetIsFull)) bool const IsFull;
	};

	nstreambuf outputStreamBuffer;
	std::ostream responseStream;
	HeaderMap headers;
	static std::string const empty;
	std::string response;
	unsigned short statusCode;

	bool SendHeaders();
};
