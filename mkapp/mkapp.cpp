#include "pch.h"

void mkquery(std::map<std::string, bool> names, std::ostream* pout);
void mkheader(std::vector<std::pair<std::string, std::string>> const& names, std::ostream* pout);

namespace {
	template<typename F>
	F* Open(F& f, char const* filePath, char const* message) {
		f.open(filePath);
		if(f) {
			return &f;
		}
		std::cerr << "cannot open \"" << filePath << "\" for " << message << std::endl;
		return nullptr;
	}
}

int main(int argc, char* argv[]) {
	// Configure the input and output files.
	std::ifstream fin;
	std::istream* pin = argc > 1 ? Open(fin, argv[1], "reading") : &std::cin;
	if(!pin) {
		return 1;
	}
	std::ofstream fout;
	std::ostream* pout = argc > 2 ? Open(fout, argv[2], "writing") : &std::cout;
	if(!pout) {
		return 1;
	}

	// Read the query names to capture.
	std::string line;
	if(!std::getline(*pin, line)) {
		return 1;
	}
	std::stringstream queryNameStream(line);
	std::map<std::string, bool> queryNames;
	using issit_t = std::istream_iterator<std::string>;
	std::transform(issit_t(queryNameStream), issit_t(), std::inserter(queryNames, queryNames.end()), [](std::string const& name) {
		if(name.back() == '+') {
			return std::make_pair(name.substr(0, name.size() - 1), true);
		} else {
			return std::make_pair(name, false);
		}
	});

	mkquery(queryNames, pout);

	// Read the header names to capture.
	if(!std::getline(*pin, line)) {
		return 1;
	}
	std::stringstream headerNameStream(line);
	std::vector<std::pair<std::string, std::string>> headerNames;
	std::transform(issit_t(headerNameStream), issit_t(), std::back_inserter(headerNames), [](std::string const& name) {
		auto i = name.find('-');
		std::string firstName = name.substr(0, i);
		auto& f = std::use_facet<std::ctype<char>>(std::locale());
		f.tolower(&firstName[0], &firstName[firstName.size()]);
		std::stringstream variableName;
		variableName << firstName;
		while(i != name.npos) {
			++i;
			auto j = name.find('-', i);
			auto nextName = name.substr(i, j - i);
			f.toupper(&nextName[0], &nextName[1]);
			f.tolower(&nextName[1], &nextName[nextName.size()]);
			variableName << nextName;
			i = j;
		}
		return std::make_pair(name, variableName.str());
	});

	mkheader(headerNames, pout);
}
