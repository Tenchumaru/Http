#pragma once

#include "StatusLines.h"
#include "HeaderMap.h"

class HttpParser {
public:
	class Exception : public std::runtime_error {
	public:
		explicit Exception(StatusLines::StatusLine const& statusLine, char const* message = nullptr) : std::runtime_error(""), statusLine(statusLine), message(message) {}
		~Exception() {}
		StatusLines::StatusLine const& get_StatusLine() const { return statusLine; }
		char const* get_Message() const { return message; }
		__declspec(property(get = get_StatusLine)) StatusLines::StatusLine const& StatusLine;
		__declspec(property(get = get_Message)) char const* const Message;

	private:
		StatusLines::StatusLine const& statusLine;
		char const* message;
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
