#include "pch.h"
#include "HttpParser.h"

using namespace std::literals;

namespace {
	constexpr size_t versionSize = 8; // Eight characters for "HTTP #.#".  See https://tools.ietf.org/html/rfc7230#appendix-B.
	// TODO:  the following are configurable.
	constexpr size_t maxNameSize = 222;
	constexpr size_t maxValueSize = 888;
	constexpr size_t maxContentLength = 1024 * 1024;
	constexpr size_t maxHeaders = 99;

	template<size_t N>
	constexpr size_t CountDigits() {
		if constexpr (N < 10) {
			return 1;
		} else {
			return CountDigits<N / 10>() + 1;
		}
	}
}

char const* HttpParser::Add(char const* begin, char const* end) {
	try {
		return (this->*fn)(begin, end);
	} catch (...) {
		Reset();
		throw;
	}
}

void HttpParser::Reset() {
	first.clear();
	fn = &HttpParser::CollectFirst;
	isComplete = false;
}

char const* HttpParser::CollectFirst(char const* p, char const* const q) {
	while (p < q) {
		if (*p == ' ') {
			// Validate it and start collecting the next part.
			if (!ValidateFirst(first)) {
				throw Exception(StatusLines::MethodNotAllowed);
			}
			next.clear();
			fn = &HttpParser::CollectNext;
			return p + 1;
		} else if (*p == '\r' || *p == '\n') {
			// Didn't find it; it's an invalid message.
			throw Exception(StatusLines::BadRequest, "malformed start line");
		}
		first += *p;
		++p;
	}
	return p;
}

char const* HttpParser::CollectNext(char const* p, char const* const q) {
	while (p < q) {
		if (*p == ' ') {
			// Validate it and start collecting the last part.
			if (!ValidateNext(next)) {
				throw Exception(StatusLines::BadRequest, "malformed start line");
			}
			last.clear();
			fn = &HttpParser::CollectLast;
			return p + 1;
		} else if (*p == '\r' || *p == '\n') {
			// Didn't find it; it's an invalid message.
			throw Exception(StatusLines::BadRequest, "malformed start line");
		}
		next += *p;
		++p;
	}
	return p;
}

char const* HttpParser::CollectLast(char const* p, char const* const q) {
	while (p < q) {
		if (*p == '\r') {
			// Skip it.
		} else if (*p == '\n') {
			// Validate it and start collecting the first header name.
			if (!ValidateLast(last)) {
				throw Exception(StatusLines::BadRequest, "malformed start line");
			}
			name.clear();
			fn = &HttpParser::CollectHeaderName;
			return p + 1;
		} else {
			last += *p;
		}
		++p;
	}
	return p;
}

char const* HttpParser::CollectHeaderName(char const* p, char const* const q) {
	// Check for too many headers.
	if (headers.size() >= maxHeaders) {
		throw Exception(StatusLines::BadRequest, "too many headers");
	}

	// Look for the end of the name (a colon).
	for (; p < q; ++p) {
		if (*p == ':') {
			// Found it; validate the name.
			if (name.empty()) {
				throw Exception(StatusLines::BadRequest, "malformed header");
			}

			// Start collecting the header value.
			value.clear();
			fn = &HttpParser::CollectHeaderValue;
			return p + 1;
		} else if (*p == '\r') {
			// Skip it.
		} else if (*p == '\n') {
			// Didn't find it.  If there is a name, it's an invalid message.
			if (!name.empty()) {
				throw Exception(StatusLines::BadRequest, "malformed header");
			}

			// Check for appropriate payload size headers.
			contentLength = ValidateDataSizeHeaders();
			if (contentLength > maxContentLength) {
				// && .49999999999999
				// : ]
				throw Exception(StatusLines::PayloadTooLarge);
			}

			// Complete the collection of data.
			isComplete = true;
			return p + 1;
		} else if (isspace(*p)) {
			if (name.empty()) {
				name += ' ';
			} else if (name != " ") {
				throw Exception(StatusLines::BadRequest, "malformed header");
			}
		} else {
			name += *p;
			if (name.size() >= maxNameSize) {
				throw Exception(StatusLines::BadRequest, "malformed header");
			}
		}
	}
	return p;
}

char const* HttpParser::CollectHeaderValue(char const* p, char const* const q) {
	while (p < q) {
		if (*p == '\n') {
			// Found it.
			if (name[0] != ' ') {
				// Add it and the header name to the collection of headers.
				headers.insert({ name, value });
			}

			// Start collecting the next header.
			name.clear();
			fn = &HttpParser::CollectHeaderName;
			return p + 1;
		} else if (*p == '\r') {
			// Skip it.
		} else if (!value.empty() || !isspace(*p)) {
			value += *p;
			if (value.size() >= maxValueSize) {
				throw Exception(StatusLines::BadRequest, "malformed header");
			}
		}
		++p;
	}
	return p;
}

bool HttpParser::ValidateVersion(std::string const& s) {
	return s.size() == versionSize &&
		s[6] == '.' &&
		isdigit(s[5]) &&
		isdigit(s[7]) &&
		strncmp(s.c_str(), "HTTP/", 5) == 0;
}

std::streamsize HttpParser::ValidateDataSizeHeaders() {
	// Get the content length or transfer encoding, if any.  These two headers are mutually exclusive.
	auto it = headers.find("Content-Length"s);
	if (it == headers.cend()) {
		// Check for a Transfer-Encoding header.
		it = headers.find("Transfer-Encoding"s);
		if (it == headers.cend()) {
			// There isn't either header; assume there's no data.
			return 0;
		} else if (_stricmp(it->second.c_str(), "chunked")) {
			throw Exception(StatusLines::BadRequest, "invalid transfer encoding");
		} else {
			// Signal a chunked transfer.
			return -1;
		}
	} else if (headers.find("Transfer-Encoding"s) != headers.cend()) {
		throw Exception(StatusLines::BadRequest, "invalid combination of content length and transfer encoding");
	} else if (!std::all_of(it->second.cbegin(), it->second.cend(), [](char ch) { return std::isdigit(ch, std::locale()); })) {
		throw Exception(StatusLines::BadRequest, "invalid content length");
	} else if (it->second.size() > CountDigits<maxContentLength>()) {
		throw Exception(StatusLines::PayloadTooLarge);
	}
	return std::stoll(it->second);
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
