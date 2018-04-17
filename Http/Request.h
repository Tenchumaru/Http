#pragma once

#include "Uri.h"

class Request {
public:
	std::string const& GetVerb() const { return verb; }
	Uri const& GetUri() const { return uri; }
	std::string const& GetVersion() const { return version; }
	std::map<std::string, std::string> const& GetHeaders() const { return headers; }
	std::string const& GetData() const { return data; }

	__declspec(property(get=GetVerb)) std::string const& Verb;
	__declspec(property(get=GetUri)) Uri const& Uri;
	__declspec(property(get=GetVersion)) std::string const& Version;
	__declspec(property(get=GetHeaders)) std::map<std::string, std::string> const& Headers;
	__declspec(property(get=GetData)) std::string const& Data;

private:
	friend class RequestParser;

	::Uri uri;
	std::string verb, version, data;
	std::map<std::string, std::string> headers;
};
