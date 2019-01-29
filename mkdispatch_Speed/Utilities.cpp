#include "pch.h"
#include "Utilities.h"

bool CollectParameter(char const*& p, size_t index, char const*& pn, char const*& qn) {
	pn = qn = p + index;
	while(*qn != '?' && *qn != '/' && *qn != '\r' && *qn != '\n') {
		++qn;
	}
	p = qn - index - 1;
	return true;
}

char const* CollectParameter(char const*& p, size_t index) {
	char const* q = p + index;
	while(*q != '?' && *q != '/' && *q != '\r' && *q != '\n') {
		++q;
	}
	p = q - index;
	return q;
}

bool CollectQuery(char const* p) {
	return *p == '?' || *p == '\r' || *p == '\n';
}

void FourZeroFour() {
	std::cout << "unexpected FourZeroFour" << std::endl;
	exit(1);
}
