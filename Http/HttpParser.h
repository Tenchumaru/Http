#pragma once

#include "HeaderMap.h"
#include "Uri.h"

class HttpParser {
public:
	HttpParser();
	virtual ~HttpParser();
	void Add(char const* p, size_t n);

protected:
	std::string first, next, last, data;
	HeaderMap headers;

	void Reset();
	bool ValidateVersion(std::string const& s);

private:
	using fn_t= void(HttpParser::*)(char const* p, char const* q);

	fn_t fn;
	std::string name, value;
	size_t contentLength;

	void CollectFirst(char const* p, char const* q);
	void CollectLast(char const* p, char const* q);
	void CollectNext(char const* p, char const* q);
	void CollectHeaderName(char const* p, char const* q);
	void CollectHeaderValue(char const* p, char const* q);
	void CollectData(char const* p, char const* q);
	virtual bool ValidateFirst(std::string const& s);
	virtual bool ValidateNext(std::string const& s);
	virtual bool ValidateLast(std::string const& s);
	virtual void HandleMessage()= 0;
};
