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
	using fn_t= char const*(HttpParser::*)(char const* p, char const* const q);

	fn_t fn;
	std::string name, value;
	size_t contentLength;

	char const* CollectFirst(char const* p, char const* const q);
	char const* CollectLast(char const* p, char const* const q);
	char const* CollectNext(char const* p, char const* const q);
	char const* CollectHeaderName(char const* p, char const* const q);
	char const* CollectHeaderValue(char const* p, char const* const q);
	char const* CollectData(char const* p, char const* const q);
	virtual bool ValidateFirst(std::string const& s);
	virtual bool ValidateNext(std::string const& s);
	virtual bool ValidateLast(std::string const& s);
	virtual void HandleMessage()= 0;
};
