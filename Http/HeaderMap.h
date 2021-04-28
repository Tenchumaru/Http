#pragma once

#ifndef _WIN32
inline int _stricmp(char const* a, char const* b) {
	while (*a && *b) {
		auto difference = tolower(*a) - tolower(*b);
		if (difference) {
			return difference;
		}
		++a;
		++b;
	}
	return *a - *b;
}
#endif

struct IgnoreCase { bool operator()(std::string const& x, std::string const& y) const { return _stricmp(x.c_str(), y.c_str()) < 0; } };

using HeaderMap = std::map<std::string, std::string, IgnoreCase>;
