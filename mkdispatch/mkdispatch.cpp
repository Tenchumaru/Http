#include "pch.h"
#include "SmPrinter.h"

namespace {
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
	bool wantsStrings = argc > 1 && (argv[1] == std::string("-s") && (--argc, ++argv));

	// Read the requests.
	std::ifstream fin;
	if(argc > 1) {
		fin.open(argv[1]);
		if(!fin) {
			std::cerr << prog << ": cannot open \"" << argv[1] << "\" for reading" << std::endl;
			return 1;
		}
	}
	std::istream& in = argc > 1 ? fin : std::cin;
	std::string s;
	Printer::vector requests;
	while(std::getline(in, s)) {
		auto it = s.find('\t');
		if(it == s.npos) {
			std::cerr << prog << ": malformed input file" << std::endl;
			return 1;
		}
		requests.push_back({ RemoveParameterNames(s.substr(0, it)), s.substr(it + 1) });
	}

	// Sort the requests.  This enables dealing with ambiguity by selecting
	// later requests to favor non-parameters over parameters.
	std::sort(requests.begin(), requests.end(), [](Printer::Request const& left, Printer::Request const& right) {
		return left.line < right.line;
	});

	// Print the Dispatcher class.
	Options options = { wantsStrings };
	SmPrinter printer;
	(static_cast<Printer&>(printer)).Print(requests, options);
}
