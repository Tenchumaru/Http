#include "pch.h"
#include "MyPrinter.h"
#include "mkapp.h"

std::string Capitalize(std::string const& s) {
	return static_cast<decltype(s[0])>(std::toupper(s[0])) + s.substr(1);
}

std::string ConstructHeaderClassName(std::set<std::string, HeaderNameLess> const& names) {
	std::string rv = "Header";
	for(auto const& name : names) {
		rv += '_';
		std::for_each(name.cbegin(), name.cend(), [&rv](char ch) {
			rv += ch == '-' ? 'X' : tolower(ch);
		});
	}
	return rv;
}

std::string ConstructQueryClassName(std::set<std::string> const& names) {
	std::string rv = "Query";
	for(auto const& name : names) {
		rv += '_';
		auto i = name.find('[');
		if(i != name.npos) {
			rv += name.substr(0, i) + '_';
		} else {
			rv += name;
		}
	}
	return rv;
}

std::string GetType(int count) {
	std::string type;
	if(count) {
		std::stringstream ss;
		ss << "xvector<" << count << '>';
		type = ss.str();
	} else {
		type = "xstring";
	}
	return type;
}

namespace {
	bool wantsStrings;

	template<typename F>
	F* Open(F& f, char const* filePath, char const* message) {
		f.open(filePath);
		if(f) {
			return &f;
		}
		std::cerr << "cannot open \"" << filePath << "\" for " << message << std::endl;
		return nullptr;
	}

	bool GetOptions(char const* prog, int& argc, char**& argv) {
		while(argc > 1 && argv[1][0] == '-') {
			switch(argv[1][1]) {
			case 's':
				wantsStrings = true;
				break;
			default:
				std::cerr << prog << ": unknown option '" << argv[1][1] << '\'' << std::endl;
				return false;
			}
			--argc;
			++argv;
		}
		return true;
	}

	std::string RemoveParameterNames(std::string const& line) {
		std::string processedLine;
		std::string::size_type i = 0, j;
		while(j = line.find(':', i), j != std::string::npos) {
			processedLine.append(line.cbegin() + i, line.cbegin() + j + 1);
			i = line.find('/', j);
			if(i == std::string::npos) {
				i = line.size();
			}
		}
		processedLine.append(line.substr(i));
		return processedLine;
	}
}

int main(int argc, char* argv[]) {
	char const* prog = strrchr(argv[0], '\\');
	prog = prog ? ++prog : argv[0];
	if(!GetOptions(prog, argc, argv)) {
		return 2;
	}

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

	*pout << "// This is an auto-generated file.  Do not edit." << std::endl;
	*pout << "#ifndef DISPATCH" << std::endl;
	using issit_t = std::istream_iterator<std::string>;
	std::string line;
	Printer::vector requests;
	std::set<std::set<std::string>> queryNamesSet;
	std::set<std::set<std::string, HeaderNameLess>> headerNamesSet;
	for(int lineNumber = 1; std::getline(*pin, line); ++lineNumber) {
		// Read the path-function pairs.
		if (!line.empty() && line.back() == '\r') {
			line.pop_back();
		}
		auto it = line.find('\t');
		if(it == line.npos) {
			std::cerr << prog << ": malformed input file at line " << lineNumber << std::endl;
			return 1;
		}
		requests.push_back({ RemoveParameterNames(line.substr(0, it)), line.substr(it + 1) });

		// Read the query names to capture.
		++lineNumber;
		if(!std::getline(*pin, line)) {
			std::cerr << prog << ": malformed input file at line " << lineNumber << std::endl;
			return 1;
		}
		if (!line.empty() && line.back() == '\r') {
			line.pop_back();
		}
		if(!line.empty()) {
			// Print the query collection function.
			std::stringstream ss(line);
			std::vector<std::string> names;
			std::copy(issit_t(ss), issit_t(), std::back_inserter(names));
			std::set<std::string> sortedNames;
			std::copy(names.begin(), names.end(), std::inserter(sortedNames, sortedNames.end()));
			if(names.size() != sortedNames.size()) {
				std::cerr << "duplicate query names at line " << lineNumber << std::endl;
				exit(1);
			}
			if(queryNamesSet.find(sortedNames) == queryNamesSet.cend()) {
				queryNamesSet.insert(sortedNames);
				requests.back().queriesFn = mkquery(sortedNames, *pout);
			} else {
				requests.back().queriesFn = ConstructQueryClassName(sortedNames);
			}
		}

		// Read the header names to capture.
		++lineNumber;
		if(!std::getline(*pin, line)) {
			std::cerr << prog << ": malformed input file at line " << lineNumber << std::endl;
			return 1;
		}
		if (!line.empty() && line.back() == '\r') {
			line.pop_back();
		}
		if(!line.empty()) {
			// Print the header collection function.
			std::stringstream ss(line);
			std::vector<std::string> names;
			std::copy(issit_t(ss), issit_t(), std::back_inserter(names));
			std::set<std::string, HeaderNameLess> sortedNames;
			std::copy(names.begin(), names.end(), std::inserter(sortedNames, sortedNames.end()));
			if(names.size() != sortedNames.size()) {
				std::cerr << "duplicate header names at line " << lineNumber << std::endl;
				exit(1);
			}
			if(headerNamesSet.find(sortedNames) == headerNamesSet.cend()) {
				headerNamesSet.insert(sortedNames);
				requests.back().headersFn = mkheader(sortedNames, *pout);
			} else {
				requests.back().headersFn = ConstructHeaderClassName(sortedNames);
			}
		}
	}

	// Print the Dispatcher class.
	*pout << "#else" << std::endl;
	Options options = { wantsStrings };
	MyPrinter().Print(requests, options, *pout);
	*pout << "#endif" << std::endl;

	return 0;
}
