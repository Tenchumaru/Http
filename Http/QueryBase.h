#pragma once

#include "xtypes.h"

class QueryBase {
public:
	QueryBase() = default;
	QueryBase(QueryBase const&) = default;
	QueryBase(QueryBase&&) = default;
	QueryBase& operator=(QueryBase const&) = default;
	QueryBase& operator=(QueryBase&&) = default;
	virtual ~QueryBase() = default;

	virtual bool CollectQueryName(char const*& p, xstring*& q) {
		while(*p != '=' && *p != '&' && *p != ' ' && *p != '#' && *p != '\r' && *p != '\n') {
			++p;
		}
		if(*p == '=') {
			++p;
			q = nullptr;
			return true;
		}
		return false;
	}

	void CollectQueryValue(char const*& p, xstring& q) {
		q.first = p;
		while(*p != '&' && *p != ' ' && *p != '#' && *p != '\r' && *p != '\n') {
			++p;
		}
		q.second = p;
		if(*p == '&') {
			++p;
		}
	}

	bool CollectQueries(char const*& p, int offset) {
		p += offset;

		// Check for early termination cases.
		if(*p == '#') {
			do {
				++p;
			} while(*p != ' ' && *p != '\r' && *p != '\n');
		}
		if(*p == ' ') {
			return true;
		}
		if(*p != '?') {
			return false;
		}
		++p;

		// Loop, expecting name-value pairs.
		xstring* q;
		while(CollectQueryName(p, q)) {
			if(q) {
				CollectQueryValue(p, *q);
			} else {
				while(*p != '&' && *p != ' ' && *p != '#' && *p != '\r' && *p != '\n') {
					++p;
				}
				if(*p == '&') {
					++p;
				} else {
					break;
				}
			}
		}

		// Ignore any fragment.
		if(*p == '#') {
			do {
				++p;
			} while(*p != ' ' && *p != '\r' && *p != '\n');
		}
		return *p == ' ';
	}
};
