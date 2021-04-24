#include "pch.h"
#include "Http.h"

bool AtEndOfPath(char ch) {
	return std::isspace(ch, std::locale()) || ch == '?' || ch == '#';
}

bool CollectParameter(char const*& p, size_t index, char const*& pn, char const*& qn) {
	pn = qn = p + index;
	while (*qn != '?' && *qn != '/' && *qn != ' ' && *qn != '\r' && *qn != '\n') {
		++qn;
	}
	p = qn - index - 1;
	return true;
}

void FourHundred(Response& response, char const* message) {
	response, message;
	// TODO
}

void FourZeroFour(Response& response) {
	response;
	// TODO
}
