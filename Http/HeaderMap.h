#pragma once

struct IgnoreCase { bool operator()(std::string const& x, std::string const& y) const { return _stricmp(x.c_str(), y.c_str()) < 0; } };

using HeaderMap= std::map<std::string, std::string, IgnoreCase>;
