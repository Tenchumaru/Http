#include "pch.h"
#include "SegPrinter.h"

namespace {
	class Node {
	public:
		Node() : index(0) {}

#pragma warning(disable: 4458) // declaration of 'segment' hides class member
		void Add(Printer::Request const& request) {
			auto i = request.line.find('/');
			if(i == std::string::npos) {
				throw std::runtime_error("invalid request");
			}
			auto segment = request.line.substr(0, i);
			auto it = children.find(segment);
			if(it == children.cend()) {
				it = children.insert({ segment, Node(segment, 0) }).first;
			}
			it->second.Add(request, i + 1);
		}
#pragma warning(default: 4458)

		void Print(Options const& options) const {
			for(auto const& pair : children) {
				pair.second.Print(options, 1, 0);
				std::cout << "\telse" << std::endl;
			}
			std::cout << "\t\t;" << std::endl;
		}

	private:
		std::map<std::string, Node> children;
		std::string segment, fn;
		size_t index;

		Node(std::string const& segment, size_t index) : segment(segment), index(index) {}

#pragma warning(disable: 4458) // declaration of '*' hides class member
		void Add(Printer::Request const& request, size_t index) {
			auto i = request.line.find('/', index);
			auto segment = request.line.substr(index, i == std::string::npos ? i : i - index);
			auto it = children.find(segment);
			if(it == children.cend()) {
				it = children.insert({ segment, Node(segment, index) }).first;
			}
			if(i == std::string::npos) {
				if(!it->second.fn.empty()) {
					throw std::runtime_error("duplicate function");
				}
				it->second.fn = request.fn;
			} else if(segment == ":") {
				it->second.Add(Printer::Request{ request.line.substr(0, index) + request.line.substr(i),request.fn }, i);
			} else {
				it->second.Add(request, i + 1);
			}
		}
#pragma warning(default: 4458)

		void Print(Options const& options, size_t indentLevel, size_t parameterCount) const {
			std::string indent(indentLevel, '\t');
			if(children.empty()) {
				if(segment == ":") {
					PrintParameter(indent, parameterCount);
					std::cout << indent << "if(";
				} else {
					std::cout << indent << "if(";
					if(!segment.empty()) {
						PrintCompare();
						std::cout << " && ";
					}
				}
				std::cout << "CollectQuery(p + " << (index + (segment == ":" ? 0 : segment.size())) << ")) {" << std::endl;
				PrintFunctionReturn(indent, parameterCount, options);
				std::cout << indent << '}' << std::endl;
			} else if(fn.empty()) {
				if(segment == ":") {
					PrintParameter(indent, parameterCount);
					std::cout << indent << "if(p[" << index << "] == '/') {" << std::endl;
				} else {
					std::cout << indent << "if(memcmp(p + " << index << ", \"" <<
						segment << "/\", " << (segment.size() + 1) << ") == 0) {" << std::endl;
				}
				for(auto const& pair : children) {
					pair.second.Print(options, indentLevel + 1, parameterCount);
					std::cout << indent << "\telse" << std::endl;
				}
				std::cout << indent << "\t\t;" << std::endl;
				std::cout << indent << '}' << std::endl;
			} else {
				if(segment == ":") {
					PrintParameter(indent, parameterCount);
					std::cout << indent << "if(p[" << index << "] == '/') {" << std::endl;
					for(auto const& pair : children) {
						pair.second.Print(options, indentLevel + 1, parameterCount);
					}
					std::cout << indent << "} else if(CollectQuery(p + " << index << ")) {" << std::endl;
					PrintFunctionReturn(indent, parameterCount, options);
					std::cout << indent << '}' << std::endl;
				} else {
					if(!segment.empty()) {
						std::cout << indent << "if(";
						PrintCompare();
						std::cout << ") {" << std::endl;
					} else {
						std::cout << indent << '{' << std::endl;
					}
					std::cout << indent << "\tif(p[" << (index + segment.size()) << "] == '/') {" << std::endl;
					for(auto const& pair : children) {
						pair.second.Print(options, indentLevel + 2, parameterCount);
					}
					std::cout << indent << "\t} else if(CollectQuery(p + " << (index + segment.size()) << ")) {" << std::endl;
					PrintFunctionReturn(indent + '\t', parameterCount, options);
					std::cout << indent << "\t}" << std::endl;
					std::cout << indent << '}' << std::endl;
				}
			}
		}

		void PrintCompare() const {
			if(segment.size() == 1) {
				std::cout << "p[" << index << "] == '" << segment << "'";
			} else {
				std::cout << "memcmp(p + " << index << ", \"" << segment << "\", " << segment.size() << ") == 0";
			}
		}

		void PrintFunctionReturn(std::string const& indent, size_t parameterCount, Options const& options) const {
			std::cout << indent << "\treturn " << fn << '(';
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
		}

		void PrintParameter(std::string const& indent, size_t& parameterCount) const {
			std::cout << indent << "char const* p" << parameterCount << " = p + " << index << ';' << std::endl;
			std::cout << indent << "char const* q" << parameterCount << " = CollectParameter(p, " << index << ");" << std::endl;
			++parameterCount;
		}
	};
}

SegPrinter::SegPrinter() {}

SegPrinter::~SegPrinter() {}

void SegPrinter::InternalPrint(vector const& requests, Options const& options) {
	Node root;
	for(auto const& request : requests) {
		root.Add(request);
	}
	root.Print(options);
}
