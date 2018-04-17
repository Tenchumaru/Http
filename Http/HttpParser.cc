#include "stdafx.h"
#include "HttpParser.h"

namespace {
	size_t const versionSize= 8; // Eight characters for "HTTP #.#".  See https://tools.ietf.org/html/rfc7230#appendix-B.
	// TODO:  the following are configurable.
	size_t const maxNameSize= 222;
	size_t const maxValueSize= 888;
	size_t const maxContentLength= 1024 * 1024;
	size_t const maxHeaders= 99;
}

HttpParser::HttpParser() : fn(&HttpParser::CollectFirst) {}

HttpParser::~HttpParser() {}

void HttpParser::Add(char const* p, size_t n) {
	return (this->*fn)(p, p + n);
}

void HttpParser::Reset() {
	first.clear();
	fn= &HttpParser::CollectFirst;
}

void HttpParser::CollectFirst(char const* p, char const* q) {
	while(p < q) {
		if(*p == ' ') {
			// Validate it and start collecting the next part.
			if(!ValidateFirst(first)) {
				throw std::runtime_error("HttpParser::CollectFirst.ValidateFirst");
			}
			next.clear();
			fn= &HttpParser::CollectNext;
			return (this->*fn)(p + 1, q);
		} else if(*p == '\r' || *p == '\n') {
			// Didn't find it; it's an invalid message.
			throw std::runtime_error("HttpParser::CollectFirst");
		}
		first += *p;
		++p;
	}
}

void HttpParser::CollectNext(char const* p, char const* q) {
	while(p < q) {
		if(*p == ' ') {
			// Validate it and start collecting the last part.
			if(!ValidateNext(next)) {
				throw std::runtime_error("HttpParser::CollectNext.ValidateNext");
			}
			last.clear();
			fn= &HttpParser::CollectLast;
			return (this->*fn)(p + 1, q);
		} else if(*p == '\r' || *p == '\n') {
			// Didn't find it; it's an invalid message.
			throw std::runtime_error("HttpParser::CollectNext");
		}
		next += *p;
		++p;
	}
}

void HttpParser::CollectLast(char const* p, char const* q) {
	while(p < q) {
		if(*p == '\r') {
			// Skip it.
		} else if(*p == '\n') {
			// Validate it and start collecting the first header name.
			if(!ValidateLast(last)) {
				throw std::runtime_error("HttpParser::CollectLast");
			}
			name.clear();
			fn= &HttpParser::CollectHeaderName;
			return (this->*fn)(p + 1, q);
		} else {
			last += *p;
		}
		++p;
	}
}

void HttpParser::CollectHeaderName(char const* p, char const* q) {
	// Check for too many headers.
	if(headers.size() > maxHeaders) {
		throw std::runtime_error("HttpParser::CollectHeaderName.maxHeaders");
	}

	// Look for the end of the name (a colon).
	while(p < q) {
		if(*p == ':') {
			// Found it; validate the name.
			if(name.empty()) {
				throw std::runtime_error("HttpParser::CollectHeaderName.name.empty");
			}

			// Start collecting the header value.
			value.clear();
			fn= &HttpParser::CollectHeaderValue;
			return (this->*fn)(p + 1, q);
		} else if(*p == '\r') {
			// Skip it.
		} else if(*p == '\n') {
			// Didn't find it.  If there is a name, it's an invalid message.
			if(!name.empty()) {
				throw std::runtime_error("HttpParser::CollectHeaderName.!name.empty");
			}

			// Get the content length, if any.
			auto const it= headers.find("Content-Length");
			contentLength= it == headers.cend() ? 0 : std::stoul(it->second);
			if(contentLength == 0) {
				// There isn't one or it's zero; assume there's no data.
				HandleMessage();

				// Start collecting the next message.
				Reset();
				return (this->*fn)(p + 1, q);
			}
			if(contentLength > maxContentLength) {
				// && .49999999999999
				// : ]
				throw std::runtime_error("HttpParser::CollectHeaderName.contentLength");
			}

			// Start collecting the data.
			data.clear();
			fn= &HttpParser::CollectData;
			return (this->*fn)(p + 1, q);
		} else {
			name += *p;
			if(name.size() >= maxNameSize) {
				throw std::runtime_error("HttpParser::CollectHeaderName.maxNameSize");
			}
		}
		++p;
	}
}

void HttpParser::CollectHeaderValue(char const* p, char const* q) {
	while(p < q) {
		if(*p == '\n') {
			// Found it; validate the name.
			if(name.empty()) {
				throw std::runtime_error("HttpParser::CollectHeaderValue.name.empty");
			}

			// Add it and the header name to the collection of headers.
			headers.insert({ name, value });

			// Start collecting the next header.
			name.clear();
			fn= &HttpParser::CollectHeaderName;
			return (this->*fn)(p + 1, q);
		} else if(*p == '\r') {
			// Skip it.
		} else if(!value.empty() || !isspace(*p)) {
			value += *p;
			if(value.size() >= maxValueSize) {
				throw std::runtime_error("HttpParser::CollectHeaderValue.maxValueSize");
			}
		}
		++p;
	}
}

void HttpParser::CollectData(char const* p, char const* q) {
	// Determine the amout of data required and available.
	auto const nRequired= contentLength - data.size();
	decltype(nRequired) const nAvailable= q - p;

	if(nAvailable >= nRequired) {
		// Consume data necessary to complete the current message.
		data.append(p, p + nRequired);
		HandleMessage();

		// Start collecting the next message.
		Reset();
		return (this->*fn)(p + nRequired, q);
	} else {
		// Consume all available data.
		data.append(p, q);
	}
}

bool HttpParser::ValidateVersion(std::string const& s) {
	if(s.size() != versionSize || strncmp(s.c_str(), "HTTP/", 5) != 0) {
		return false;
	}
	if(!isdigit(s[5]) || s[6] != '.' || !isdigit(s[7])) {
		return false;
	}
	return true;
}

bool HttpParser::ValidateFirst(std::string const& s) {
	return !s.empty();
}

bool HttpParser::ValidateNext(std::string const& s) {
	return !s.empty();
}

bool HttpParser::ValidateLast(std::string const& s) {
	return !s.empty();
}
