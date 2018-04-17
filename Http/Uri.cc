#include "stdafx.h"
#include "Uri.h"

inline int FromHexDigit(char ch) {
	return ch < 'A' ? ch - '0' : ch < 'a' ? ch - 'A' + 10 : ch - 'a' + 10;
}

bool Uri::DecodeResource(std::string& resource, bool isPlusSpace) {
	if(isPlusSpace) {
		std::replace(resource.begin(), resource.end(), '+', ' ');
	}
	std::string decodedResource;
	for(size_t i= 0, n= resource.size(); i < n;) {
		auto const j= resource.find('%', i);
		if(j != resource.npos) {
			decodedResource.append(resource, i, j - i);
			if(resource.size() < j + 2 || !isxdigit(resource[j + 1]) || !isxdigit(resource[j + 2])) {
				return false;
			}
			decodedResource += static_cast<char>((FromHexDigit(resource[j + 1]) << 4) + FromHexDigit(resource[j + 2]));
			i= j + 3;
		} else {
			decodedResource.append(resource, i, resource.npos);
			break;
		}
	}
	resource.swap(decodedResource);
	return true;
}

bool Uri::ParseQuery(std::string const& rawQuery, map& query) {
	std::string::size_type ampersand= 0;
	pair nameValue;
	for(std::string::size_type next; next= rawQuery.find('&', ampersand), next != rawQuery.npos; ampersand= next + 1) {
		if(ParseNameValue(rawQuery.cbegin() + ampersand, rawQuery.cbegin() + next, nameValue)) {
			query.insert(nameValue);
		} else {
			return false;
		}
	}
	if(ParseNameValue(rawQuery.cbegin() + ampersand, rawQuery.cend(), nameValue)) {
		query.insert(nameValue);
	} else {
		return false;
	}
	return true;
}

bool Uri::ParseNameValue(std::string::const_iterator first, std::string::const_iterator last, pair& nameValue) {
	auto it= std::find(first, last, '=');
	nameValue.first= std::string(first, it);
	if(!DecodeResource(nameValue.first, true)) {
		return false;
	}
	nameValue.second= it != last ? std::string(it + 1, last) : std::string();
	if(!DecodeResource(nameValue.second, true)) {
		return false;
	}
	return true;
}

bool Uri::Create(std::string const& s, Uri& uri) {
	if(s.empty()) {
		// The string is empty.
		return false;
	}
	auto first= s.cbegin();
	auto limit= s.cend();

	// Check for and extract the scheme.
	std::string scheme;
	auto it= std::find(first, limit, ':');
	if(it != limit && std::all_of(first, it, [](char ch) { return isalnum(ch) || ch == '+' || ch == '.' || ch == '-'; })) {
		scheme= std::string(first, it);
		first= it + 1;
	}

	// Check for and extract the authority.
	std::string authority, host, port;
	if(limit - first >= 2) {
		it= std::find_if(first, limit, [](char ch) { return ch != '/'; });
		if(it == first + 2) {
			first= it;
			it= std::find_if(first, limit, [](char ch) { return ch == '/' || ch == '?' || ch == '#'; });
			authority= std::string(first, it);
			first= it;

			// Extract the host and optional port.
			it= std::find(authority.cbegin(), authority.cend(), '@');
			if(it == authority.cend()) {
				// There isn't a user name or password.
				it= authority.cbegin();
			} else {
				// There is a user name or password; skip them.
				++it;
			}
			host= std::string(it, authority.cend());
			it= std::find(host.cbegin(), host.cend(), ':');
			if(it != host.cend()) {
				port= std::string(it + 1, host.cend());
				host.erase(it, host.cend());
			} else if(scheme == "http") {
				port= "80";
			} else if(scheme == "https") {
				port= "443";
			}
		}
	}

	// Check for and extract the fragment.
	std::string fragment;
	it= std::find(first, limit, '#');
	if(it != limit) {
		fragment= std::string(it + 1, limit);
		if(!DecodeResource(fragment, false)) {
			return false;
		}
		limit= it;
	}

	// Check for and extract the raw query.
	std::string rawQuery;
	Uri::map query;
	it= std::find(first, limit, '?');
	if(it != limit) {
		rawQuery= std::string(it + 1, limit);
		limit= it;

		// Create the query collection.
		if(!ParseQuery(rawQuery, query)) {
			return false;
		}
	}

	// Extract the path.
	std::string path= std::string(first, limit);
	if(!DecodeResource(path, false)) {
		return false;
	}

	// Set the components of the URI.
	uri.scheme.swap(scheme);
	uri.authority.swap(authority);
	uri.host.swap(host);
	uri.port.swap(port);
	uri.fragment.swap(fragment);
	uri.rawQuery.swap(rawQuery);
	uri.query.swap(query);
	uri.path.swap(path);
	return true;
}
