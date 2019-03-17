#pragma once

struct HeaderNameLess {
	bool operator()(std::string const& left, std::string const& right) const {
		return _strcmpi(left.c_str(), right.c_str()) < 0;
	}
};

std::string Capitalize(std::string const& s);
std::string ConstructHeaderClassName(std::set<std::string, HeaderNameLess> const& names);
std::string ConstructQueryClassName(std::set<std::string> const& names);
std::string GetType(int count);
std::string mkquery(std::set<std::string> const& names, std::ostream& out);
std::string mkheader(std::set<std::string, HeaderNameLess> const& names, std::ostream& out);
