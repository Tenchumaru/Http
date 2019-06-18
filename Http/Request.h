#pragma once

#include "HeaderMap.h"
#include "Uri.h"

class Request {
public:
	Request() = default;
	Request(Request const&) = delete;
	Request(Request&&) = default;
	Request& operator=(Request const&) = delete;
	Request& operator=(Request&&) = default;
	~Request() = default;

	std::string const& GetVerb() const { return verb; }
	Uri const& GetUri() const { return uri; }
	int GetVersion() const { return version; }
	HeaderMap const& GetHeaders() const { return headers; }
	std::string const& GetData() const { return data; }

	__declspec(property(get = GetVerb)) std::string const& Verb;
	__declspec(property(get = GetUri)) Uri const& Uri;
	__declspec(property(get = GetVersion)) int Version;
	__declspec(property(get = GetHeaders)) HeaderMap const& Headers;
	__declspec(property(get = GetData)) std::string const& Data;

private:
	friend class RequestParser;

	::Uri uri;
	std::string verb, data;
	HeaderMap headers;
	int version;
};
