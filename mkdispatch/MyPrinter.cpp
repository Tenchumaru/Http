#include "pch.h"
#include "MyPrinter.h"

namespace {
	struct Group {
		using vector = MyPrinter::vector;

		Group(vector const& requests) {
			// Determine the common prefix for the requests.
			prefix = requests.cbegin()->line;
			for(auto const& request : requests) {
				auto pair = std::mismatch(prefix.cbegin(), prefix.cend(), request.line.cbegin(), request.line.cend());
				prefix.erase(pair.first, prefix.cend());
			}

			// Split the requests into groups based on the first character
			// after the common prefix.
			std::map<char, vector> requestGroups;
			auto index = prefix.size();
			for(auto const& request : requests) {
				if(request.line == prefix) {
					fn = request.fn;
				} else {
					requestGroups[request.line[index]].push_back(request);
				}
			}
			for(auto const& pair : requestGroups) {
				children.insert({ pair.first, Group(pair.second) });
			}
		}

		void Print(Options const& options, size_t indentLevel, size_t index, size_t parameterCount) const {
			std::string indent(indentLevel, '\t');
			size_t printedParameterNumber;
			if(index == prefix.size()) {
				printedParameterNumber = 0;
				std::cout << indent << '{' << std::endl;
			} else {
				printedParameterNumber = parameterCount + 1;
				if(!PrintCompare(indent, index, parameterCount)) {
					printedParameterNumber = 0;
				}
			}
			if(!children.empty()) {
				if(std::any_of(children.cbegin(), children.cend(), [](auto const& pair) { return pair.first != ':'; })) {
					std::cout << indent << "\tswitch(p[" << prefix.size() << "]) {" << std::endl;
					for(auto const& pair : children) {
						if(pair.first == ':') {
							continue;
						}
						std::cout << indent << "\tcase '" << pair.first << "':" << std::endl;
						pair.second.Print(options, indentLevel + 2, prefix.size() + 1, parameterCount);
						std::cout << indent << "\t\tbreak;" << std::endl;
					}
					std::cout << indent << "\t}" << std::endl;
				}
				auto it = children.find(':');
				if(it != children.cend()) {
					std::cout << indent << "\tchar const* r" << parameterCount << " = p;" << std::endl;
					std::cout << indent << "\tif(CollectParameter(p, " << prefix.size() <<
						", p" << parameterCount << ", q" << parameterCount << ")) {" << std::endl;
					it->second.Print(options, indentLevel + 2, prefix.size() + 1, parameterCount + 1);
					std::cout << indent << "\t}" << std::endl;
					std::cout << indent << "\tp = r" << parameterCount << ';' << std::endl;
				}
			}
			if(!fn.empty()) {
				std::cout << indent << "\tif(CollectQuery(p + " << prefix.size() << ")) {" << std::endl;
				std::cout << indent << "\t\treturn " << fn << '(';
				for(decltype(parameterCount) i = 0; i < parameterCount; ++i) {
					if(options.wantsStrings) {
						std::cout << "xstring(p" << i << ", " << 'q' << i << ')';
					} else {
						std::cout << 'p' << i << ", " << 'q' << i;
					}
					if(i + 1 < parameterCount) {
						std::cout << ", ";
					}
				}
				std::cout << ");" << std::endl;
				std::cout << indent << "\t}" << std::endl;
			}
			std::cout << indent << '}' << std::endl;
			if(printedParameterNumber) {
				std::cout << indent << "p = r" << (printedParameterNumber - 1) << "; }" << std::endl;
			}
		}

		bool PrintCompare(std::string const& indent, size_t const& index, size_t& parameterCount) const {
			auto substr = prefix.substr(index);
			auto i = substr.find(':');
			if(i == std::string::npos) {
				std::cout << indent << "if(";
				PrintCompare(index, substr, substr.size());
				std::cout << ") {" << std::endl;
				return false;
			} else {
				std::cout << indent << "{ char const* r" << parameterCount << " = p;" << std::endl;
				std::cout << indent << "if(";
				PrintCompare(index, substr.substr(0, i), i);
				decltype(i) j = 0;
				do {
					std::cout << " && CollectParameter(p, " << (index + i) << ", p" << parameterCount << ", q" << parameterCount << ')';
					j = substr.find(':', ++i);
					if(j == std::string::npos) {
						j = substr.size();
					}
					std::cout << " && ";
					PrintCompare(index + i, substr.substr(i, j - i), j - i);
					++parameterCount;
					i = j;
				} while(j < substr.size());
				std::cout << ") {" << std::endl;
				return true;
			}
		}

		void PrintCompare(size_t const index, std::string const& s, size_t count) const {
			switch(count) {
			case 0:
				std::cout << "true";
				break;
			case 1:
				std::cout << "p[" << index << "] == '" << s << '\'';
				break;
			default:
				std::cout << "memcmp(p + " << index << ", \"" << s << "\", " << count << ") == 0";
			}
		}

		std::map<char, Group> children;
		std::string prefix, fn;
	};
}

MyPrinter::MyPrinter() {}

MyPrinter::~MyPrinter() {}

void MyPrinter::InternalPrint(vector const& requests, Options const& options) {
	// Split the requests into groups based on the first character.
	std::map<char, vector> requestGroups;
	for(auto const& request : requests) {
		requestGroups[request.line[0]].push_back(request);
	}
	std::map<char, Group> groups;
	for(auto const& pair : requestGroups) {
		groups.insert({ pair.first, Group(pair.second) });
	}

	// Determine the maximum number of parameters.
	decltype(std::count("", "", ':')) nparameters = 0;
	for(auto const& request : requests) {
		nparameters = std::max(nparameters, std::count(request.line.cbegin(), request.line.cend(), ':'));
	}

	// Print parameter declarations.
	for(decltype(nparameters) i = 0; i < nparameters; ++i) {
		std::cout << "\tchar const* p" << i << ';' << std::endl;
		std::cout << "\tchar const* q" << i << ';' << std::endl;
	}

	// Print the Dispatch function code.
	std::cout << "\tswitch(*p) {" << std::endl;
	for(auto const& pair : groups) {
		std::cout << "\tcase '" << pair.first << "':" << std::endl;
		pair.second.Print(options, 2, 1, 0);
		std::cout << "\t\tbreak;" << std::endl;
	}
	std::cout << "\t}" << std::endl;
}
