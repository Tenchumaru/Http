#include "pch.h"
#include "../mkdispatch/MyPrinter.h"

void mkquery(std::map<std::string, bool> names, std::ostream* pout);
void mkheader(std::vector<std::pair<std::string, std::string>> const& names, std::ostream* pout);

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

	using issit_t = std::istream_iterator<std::string>;
	std::string line;
	Printer::vector requests;
	for(int lineNumber = 1; std::getline(*pin, line); ++lineNumber) {
		// Read the path-function pairs.
		auto it = line.find('\t');
		if(it == line.npos) {
			std::cerr << prog << ": malformed input file at line " << lineNumber << std::endl;
			return 1;
		}
		requests.push_back({ RemoveParameterNames(line.substr(0, it)), line.substr(it + 1) });

		// Read the query names to capture.
		if(!std::getline(*pin, line)) {
			std::cerr << prog << ": malformed input file at line " << lineNumber << std::endl;
			return 1;
		}
		if(!line.empty()) {
			// Print the query collection function.
			std::stringstream queryNameStream(line);
			std::map<std::string, bool> queryNames;
			std::transform(issit_t(queryNameStream), issit_t(), std::inserter(queryNames, queryNames.end()), [](std::string const& name) {
				if(name.back() == '+') {
					return std::make_pair(name.substr(0, name.size() - 1), true);
				} else {
					return std::make_pair(name, false);
				}
			});
			mkquery(queryNames, pout);
		}

		// Read the header names to capture.
		if(!std::getline(*pin, line)) {
			std::cerr << prog << ": malformed input file at line " << lineNumber << std::endl;
			return 1;
		}
		if(!line.empty()) {
			// Print the header collection function.
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
	}

	// Print the Dispatcher class.
	Options options = { wantsStrings };
	MyPrinter().Print(requests, options);

	return 0;
}
