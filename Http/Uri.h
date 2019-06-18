#pragma once

class Uri {
public:
	using map= std::unordered_map<std::string, std::string>;
	using pair= std::pair<std::string, std::string>;

	Uri() = default;
	Uri(Uri const&) = default;
	Uri(Uri&&) = default;
	Uri& operator=(Uri const&) = default;
	Uri& operator=(Uri&&) = default;
	~Uri() = default;

	static bool Create(std::string const& s, Uri& uri);
	static bool DecodeResource(std::string& resource, bool isPlusSpace);
	static bool ParseQuery(std::string const& rawQuery, map& query);
	static bool ParseNameValue(std::string::const_iterator first, std::string::const_iterator last, pair& nameValue);
	std::string const& GetScheme() const { return scheme; }
	std::string const& GetAuthority() const { return authority; }
	std::string const& GetHost() const { return host; }
	std::string const& GetPort() const { return port; }
	std::string const& GetPath() const { return path; }
	std::string const& GetRawQuery() const { return rawQuery; }
	map const& GetQuery() const { return query; }
	std::string const& GetFragment() const { return fragment; }

	__declspec(property(get=GetScheme)) std::string const& Scheme;
	__declspec(property(get=GetAuthority)) std::string const& Authority;
	__declspec(property(get=GetHost)) std::string const& Host;
	__declspec(property(get=GetPort)) std::string const& Port;
	__declspec(property(get=GetPath)) std::string const& Path;
	__declspec(property(get=GetRawQuery)) std::string const& RawQuery;
	__declspec(property(get=GetQuery)) map const& Query;
	__declspec(property(get=GetFragment)) std::string const& Fragment;

private:
	std::string scheme, authority, host, port, path, rawQuery, fragment;
	map query;
};

