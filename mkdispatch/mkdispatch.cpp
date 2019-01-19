#include "pch.h"
#include "Options.h"
#include "MyPrinter.h"
#include "SegPrinter.h"
#include "SmPrinter.h"

namespace {
	bool wantsStrings;
	std::shared_ptr<Printer> printer;

	bool SetPrinter(std::string const& name) {
		if(name == "my") {
			printer = std::make_shared<MyPrinter>();
		} else if(name == "seg" || name == "segment") {
			printer = std::make_shared<SegPrinter>();
		} else if(name == "sm" || name == "state") {
			printer = std::make_shared<SmPrinter>();
		} else {
			return false;
		}
		return true;
	}

	bool GetOptions(char const* prog, int& argc, char**& argv) {
		printer = std::make_shared<MyPrinter>();
		while(argc > 1 && argv[1][0] == '-') {
			switch(argv[1][1]) {
			case 'p':
				switch(argv[1][2]) {
				case ':':
				case '=':
					if(!SetPrinter(argv[1] + 3)) {
						return false;
					}
					break;
				default:
					--argc;
					++argv;
					if(argc < 2 || !SetPrinter(argv[1])) {
						return false;
					}
				}
				break;
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
		std::cerr << "usage: " << prog << " [-p printer] [-s]" << std::endl;
		return 2;
	}

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
	printer->Print(requests, options);
}
