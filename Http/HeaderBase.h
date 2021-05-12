#pragma once

#include "TcpSocket.h"
#include "ClosableResponse.h"
#include "xtypes.h"

class HeaderBase {
public:
	HeaderBase() = default;
	HeaderBase(HeaderBase const&) = delete;
	HeaderBase(HeaderBase&&) noexcept = default;
	HeaderBase& operator=(HeaderBase const&) = delete;
	HeaderBase& operator=(HeaderBase&&) noexcept = default;
	virtual ~HeaderBase() = default;

	virtual bool CollectHeaderName(char const*& p, xstring*& q) {
		if (_strnicmp(p, "Expect:", 7) == 0) {
			p += 8;
			q = &expect;
			return true;
		}
		while (*p != ':' && *p != '\r' && *p != '\n') {
			++p;
		}
		if (*p == ':') {
			++p;
			q = nullptr;
			return true;
		}
		return false;
	}

	void CollectHeaderValue(char const*& p, xstring& q) {
		while (*p == ' ' || *p == '\t') {
			++p;
		}
		q.first = p;
		while (*p != '\r' && *p != '\n') {
			++p;
		}
		q.second = p;
		while (q.second[-1] == ' ' || q.second[-1] == '\t' && q.first < q.second) {
			--q.second;
		}
		if (*p == '\r') {
			++p;
		}
		if (*p == '\n') {
			++p;
		}
	}

	bool CollectHeaders(char const*& p) {
		// Parse the HTTP version from the rest of the request line.
		if (memcmp(p, " HTTP/", 6)) {
			return false;
		}
		p += 6;
		if (!std::isdigit(*p, std::locale()) || p[1] != '.' || !std::isdigit(p[2], std::locale())) {
			return false;
		}
		majorVersion = *p - '0';
		minorVersion = p[2] - '0';
		p += 3;
		if (*p == '\r') {
			++p;
		}
		if (*p != '\n') {
			return false;
		}
		++p;

		// Check for early termination cases.
		if (*p == '\r' || *p == '\n') {
			return true;
		}

		// Loop, expecting name-value pairs.
		xstring* q;
		while (CollectHeaderName(p, q)) {
			if (q) {
				CollectHeaderValue(p, *q);
			} else {
				while (*p != '\r' && *p != '\n') {
					++p;
				}
				if (*p == '\r') {
					++p;
				}
				if (*p == '\n') {
					++p;
				}
				if (*p == '\r' || *p == '\n') {
					break;
				}
			}
		}

		// Check for final termination.
		if (*p == '\r') {
			++p;
		}
		if (*p == '\n') {
			++p;
			return true;
		}
		return false;
	}

	bool HandleExpectation(TcpSocket& socket) {
		bool rv{};
		if (expect != xstring{}) {
			std::array<char, Response::MinimumHeaderBufferSize> continueBuffer;
			ClosableResponse continueResponse(socket, continueBuffer.data(), continueBuffer.data() + continueBuffer.size());
			if (_strnicmp(expect.first, "100-continue", 12) == 0) {
				continueResponse.WriteStatus(StatusLines::Continue);
				rv = true;
			} else {
				continueResponse.WriteStatus(StatusLines::ExpectationFailed);
			}
			continueResponse.Close();
		}
		return rv;
	}

private:
	unsigned short majorVersion{};
	unsigned short minorVersion{};
	xstring expect{};
};
