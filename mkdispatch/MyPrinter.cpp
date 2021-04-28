#include "pch.h"
#include "MyPrinter.h"

using namespace std::literals;

namespace {
	struct Group {
		using vector = MyPrinter::vector;

		Group(vector const& requests, std::ostream& out) : out(out) {
			// Determine the common prefix for the requests.
			prefix = requests.cbegin()->line;
			for (auto const& request : requests) {
				auto pair = std::mismatch(prefix.cbegin(), prefix.cend(), request.line.cbegin(), request.line.cend());
				prefix.erase(pair.first, prefix.cend());
			}

			// Split the requests into groups based on the first character
			// after the common prefix.
			std::map<char, vector> requestGroups;
			auto index = prefix.size();
			for (auto const& request : requests) {
				if (request.line == prefix) {
					fn = request.fn;
					queriesFn = request.queriesFn;
					headersFn = request.headersFn;
				} else {
					requestGroups[request.line[index]].push_back(request);
				}
			}
			for (auto const& pair : requestGroups) {
				children.insert({ pair.first, Group(pair.second, out) });
			}
		}

		void Print(Options const& options, size_t indentLevel, size_t index, size_t parameterCount) const {
			std::string indent(indentLevel, '\t');
			size_t printedParameterNumber;
			if (index == prefix.size()) {
				printedParameterNumber = 0;
				out << indent << '{' << std::endl;
			} else {
				printedParameterNumber = parameterCount + 1;
				if (!PrintCompare(indent, index, parameterCount)) {
					printedParameterNumber = 0;
				}
			}
			if (!children.empty()) {
				if (std::any_of(children.cbegin(), children.cend(), [](auto const& pair) { return pair.first != ':'; })) {
					out << indent << "\tswitch(p[" << prefix.size() << "]) {" << std::endl;
					for (auto const& pair : children) {
						if (pair.first == ':') {
							continue;
						}
						out << indent << "\tcase '" << pair.first << "':" << std::endl;
						pair.second.Print(options, indentLevel + 2, prefix.size() + 1, parameterCount);
						out << indent << "\t\tbreak;" << std::endl;
					}
					out << indent << "\t}" << std::endl;
				}
				auto it = children.find(':');
				if (it != children.cend()) {
					out << indent << "\tchar const* r" << parameterCount << " = p;" << std::endl;
					out << indent << "\tif(CollectParameter(p, " << prefix.size() <<
						", p" << parameterCount << ", q" << parameterCount << ")) {" << std::endl;
					it->second.Print(options, indentLevel + 2, prefix.size() + 1, parameterCount + 1);
					out << indent << "\t}" << std::endl;
					out << indent << "\tp = r" << parameterCount << ';' << std::endl;
				}
			}
			if (!fn.empty()) {
				auto returnKeyword = options.wantsAsynchronous ? "co_await" : "return";
				auto returnStatement = options.wantsAsynchronous ? indent + "\t\t\tco_return;\n" : "";
				auto asyncSuffix = options.wantsAsynchronous ? "Async" : "";
				out << indent << "\tif(AtEndOfPath(p[" << prefix.size() << "])) {" << std::endl;
				out << indent << "\t\t" << (queriesFn.empty() ? "QueryBase" : queriesFn) << " queries;" << std::endl;
				out << indent << "\t\tif(!queries.CollectQueries(p, " << prefix.size() << ")) {" << std::endl;
				out << indent << "\t\t\t" << returnKeyword << " FourHundred" << asyncSuffix << "(response, \"bad query string\");" << std::endl;
				out << returnStatement;
				out << indent << "\t\t}" << std::endl;
				out << indent << "\t\t" << (headersFn.empty() ? "HeaderBase" : headersFn) << " headers;" << std::endl;
				out << indent << "\t\tif(!headers.CollectHeaders(p)) {" << std::endl;
				out << indent << "\t\t\t" << returnKeyword << " FourHundred" << asyncSuffix << "(response, \"bad headers\");" << std::endl;
				out << returnStatement;
				out << indent << "\t\t}" << std::endl;
				if (fn.back() == '+') {
					out << indent << "\t\tif(!headers.ContentLength.first) {" << std::endl;
					out << indent << "\t\t\t" << returnKeyword << " FourHundred" << asyncSuffix << "(response, \"no content length header\");" << std::endl;
					out << returnStatement;
					out << indent << "\t\t}" << std::endl;
					out << indent << "\t\tchar* end_;" << std::endl;
					out << indent << "\t\tauto size = std::strtol(headers.ContentLength.first, &end_, 10);" << std::endl;
					out << indent << "\t\tif(size == LONG_MAX || size == LONG_MIN || end_ != headers.ContentLength.second) {" << std::endl;
					out << indent << "\t\t\t" << returnKeyword << " FourHundred" << asyncSuffix << "(response, \"bad content length header\");" << std::endl;
					out << returnStatement;
					out << indent << "\t\t}" << std::endl;
				}
				if (options.wantsAsynchronous) {
					out << indent << "\t\tco_await ";
				} else {
					out << indent << "\t\treturn ";
				}
				std::vector<std::string> parameters;
				for (decltype(parameterCount) i = 0; i < parameterCount; ++i) {
					std::stringstream ss;
					if (options.wantsStrings) {
						ss << "xstring(p" << i << ", " << 'q' << i << ')';
					} else {
						ss << 'p' << i << ", " << 'q' << i;
					}
					parameters.push_back(ss.str());
				}
				if (!queriesFn.empty()) {
					parameters.push_back("std::move(queries)");
				}
				if (!headersFn.empty()) {
					parameters.push_back("std::move(headers)");
				}
				if (fn.back() == '+') {
					parameters.push_back(asyncSuffix + "Body(body, next, size, socket)"s);
				}
				parameters.push_back("response");
				out << (fn.back() == '+' ? fn.substr(0, fn.size() - 1) : fn);
				out << '(';
				bool isFirst = true;
				for (auto const& parameter : parameters) {
					if (isFirst) {
						isFirst = false;
					} else {
						out << ", ";
					}
					out << parameter;
				}
				out << ");" << std::endl;
				if (options.wantsAsynchronous) {
					out << indent << "\t\tco_return;" << std::endl;
				}
				out << indent << "\t}" << std::endl;
			}
			out << indent << '}' << std::endl;
			if (printedParameterNumber) {
				out << indent << "p = r" << (printedParameterNumber - 1) << "; }" << std::endl;
			}
		}

		bool PrintCompare(std::string const& indent, size_t const& index, size_t& parameterCount) const {
			auto substr = prefix.substr(index);
			auto i = substr.find(':');
			if (i == std::string::npos) {
				out << indent << "if(";
				PrintCompare(index, substr, substr.size());
				out << ") {" << std::endl;
				return false;
			} else {
				out << indent << "{ char const* r" << parameterCount << " = p;" << std::endl;
				out << indent << "if(";
				PrintCompare(index, substr.substr(0, i), i);
				decltype(i) j = 0;
				do {
					out << " && CollectParameter(p, " << (index + i) << ", p" << parameterCount << ", q" << parameterCount << ')';
					j = substr.find(':', ++i);
					if (j == std::string::npos) {
						j = substr.size();
					}
					out << " && ";
					PrintCompare(index + i, substr.substr(i, j - i), j - i);
					++parameterCount;
					i = j;
				} while (j < substr.size());
				out << ") {" << std::endl;
				return true;
			}
		}

		void PrintCompare(size_t const index, std::string const& s, size_t count) const {
			switch (count) {
			case 0:
				out << "true";
				break;
			case 1:
				out << "p[" << index << "] == '" << s << '\'';
				break;
			default:
				out << "memcmp(p + " << index << ", \"" << s << "\", " << count << ") == 0";
			}
		}

		std::ostream& out;
		std::map<char, Group> children;
		std::string prefix, fn, queriesFn, headersFn;
	};
}

MyPrinter::MyPrinter() {}

MyPrinter::~MyPrinter() {}

void MyPrinter::InternalPrint(vector const& requests, Options const& options, std::ostream& out) {
	// Split the requests into groups based on the first character.
	std::map<char, vector> requestGroups;
	for (auto const& request : requests) {
		requestGroups[request.line[0]].push_back(request);
	}
	std::map<char, Group> groups;
	for (auto const& pair : requestGroups) {
		groups.insert({ pair.first, Group(pair.second, out) });
	}

	// Determine the maximum number of parameters.
	decltype(std::count("", "", ':')) nparameters = 0;
	for (auto const& request : requests) {
		nparameters = std::max(nparameters, std::count(request.line.cbegin(), request.line.cend(), ':'));
	}

	// Print parameter declarations.
	for (decltype(nparameters) i = 0; i < nparameters; ++i) {
		out << "\tchar const* p" << i << ';' << std::endl;
		out << "\tchar const* q" << i << ';' << std::endl;
	}

	// Print the Dispatch function code.
	out << "\tswitch(*p) {" << std::endl;
	for (auto const& pair : groups) {
		out << "\tcase '" << pair.first << "':" << std::endl;
		pair.second.Print(options, 2, 1, 0);
		out << "\t\tbreak;" << std::endl;
	}
	out << "\t}" << std::endl;
}
