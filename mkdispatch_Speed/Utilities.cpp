#include "pch.h"
#include "Utilities.h"

TestResponse::TestResponse(ClientSocket& socket) : Response(date, socket, nullptr, nullptr) {}

char const* CollectParameter(char const*& p, size_t index) {
	char const* q = p + index;
	while (*q != '?' && *q != '/' && *q != '\r' && *q != '\n') {
		++q;
	}
	p = q - index;
	return q;
}

bool CollectQueries(char const* p) {
	return *p == '?' || *p == '\r' || *p == '\n';
}
