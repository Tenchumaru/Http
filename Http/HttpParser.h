#pragma once

#include "StatusLines.h"
#include "HeaderMap.h"

class HttpParser {
public:
	class Exception : public std::runtime_error {
	public:
		explicit Exception(StatusLines::StatusLine const& statusLine) : std::runtime_error("HttpParser"), statusLine(statusLine) {}
		~Exception() {}
		StatusLines::StatusLine const& GetStatusLine() const { return statusLine; }
		__declspec(property(get = GetStatusLine)) StatusLines::StatusLine const& StatusLine;

	private:
		StatusLines::StatusLine const& statusLine;
	};

	HttpParser() = default;
	HttpParser(HttpParser const&) = delete;
	HttpParser(HttpParser&&) noexcept = default;
	HttpParser& operator=(HttpParser const&) = delete;
	HttpParser& operator=(HttpParser&&) noexcept = default;
	virtual ~HttpParser() = default;

	char const* Add(char const* begin, char const* end);

protected:
	std::string first, next, last, data;
	HeaderMap headers;
	bool isComplete{};

	void Reset();
	bool ValidateVersion(std::string const& s);

private:
	using fn_t = char const* (HttpParser::*)(char const* p, char const* const q);

	fn_t fn = &HttpParser::CollectFirst;
	std::string name, value;
	size_t contentLength{};

	char const* CollectFirst(char const* p, char const* const q);
	char const* CollectLast(char const* p, char const* const q);
	char const* CollectNext(char const* p, char const* const q);
	char const* CollectHeaderName(char const* p, char const* const q);
	char const* CollectHeaderValue(char const* p, char const* const q);
	char const* CollectData(char const* p, char const* const q);
	virtual bool ValidateFirst(std::string const& s);
	virtual bool ValidateNext(std::string const& s);
	virtual bool ValidateLast(std::string const& s);
};
