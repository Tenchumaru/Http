#include "pch.h"
#include "SegPrinter.h"

namespace {
	class Node {
	public:
		Node(std::ostream& out) : out(out), index(0) {}

#pragma warning(disable: 4458) // declaration of 'segment' hides class member
		void Add(Printer::Request const& request) {
			auto i = request.line.find('/');
			if(i == std::string::npos) {
				throw std::runtime_error("invalid request");
			}
			auto segment = request.line.substr(0, i);
			auto it = children.find(segment);
			if(it == children.cend()) {
				it = children.insert({ segment, Node(out, segment, 0) }).first;
			}
			it->second.Add(request, i + 1);
		}
#pragma warning(default: 4458)

		void Collapse() {
			while(fn.empty() && segment != ":" && children.size() == 1 && children.cbegin()->second.segment != ":") {
				segment += '/' + children.cbegin()->second.segment;
				fn = children.cbegin()->second.fn;
				decltype(children) swap;
				std::swap(swap, children.begin()->second.children);
				std::swap(children, swap);
			}
			for(auto& pair : children) {
				pair.second.Collapse();
			}
		}

		void Print(Options const& options) const {
			PrintChildren(options, 0, 0, "");
		}

	private:
		struct NodeLess {
			bool operator()(std::string const& left, std::string const& right) const {
				if(left == ":") {
					return false;
				} else if(left == "") {
					return right == ":";
				} else if(right == "" || right == ":") {
					return true;
				}
				return left < right;
			}
		};

		std::ostream& out;
		std::map<std::string, Node, NodeLess> children;
		std::string segment, fn;
		size_t index;

		Node(std::ostream& out, std::string const& segment, size_t index) : out(out), segment(segment), index(index) {}

#pragma warning(disable: 4458) // declaration of '*' hides class member
		void Add(Printer::Request const& request, size_t index) {
			auto i = request.line.find('/', index);
			auto segment = request.line.substr(index, i == std::string::npos ? i : i - index);
			auto it = children.find(segment);
			if(it == children.cend()) {
				it = children.insert({ segment, Node(out, segment, index) }).first;
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
				size_t printedParameterNumber;
				if(segment == ":") {
					printedParameterNumber = PrintParameter(indent, parameterCount);
					out << indent << "if(";
				} else {
					printedParameterNumber = 0;
					out << indent << "if(";
					if(!segment.empty()) {
						PrintCompare();
						out << " && ";
					}
				}
				out << "CollectQueries(p + " << (index + (segment == ":" ? 0 : segment.size())) << ")) {" << std::endl;
				PrintFunctionReturn(indent, parameterCount, options);
				out << indent << '}' << std::endl;
				if(printedParameterNumber) {
					out << indent << "p = r" << (printedParameterNumber - 1) << ';' << std::endl;
				}
			} else if(fn.empty()) {
				size_t printedParameterNumber;
				if(segment == ":") {
					printedParameterNumber = PrintParameter(indent, parameterCount);
					out << indent << "if(p[" << index << "] == '/') {" << std::endl;
				} else {
					printedParameterNumber = 0;
					out << indent << "if(memcmp(p + " << index << ", \"" <<
						segment << "/\", " << (segment.size() + 1) << ") == 0) {" << std::endl;
				}
				PrintChildren(options, indentLevel, parameterCount, indent);
				out << indent << '}' << std::endl;
				if(printedParameterNumber) {
					out << indent << "p = r" << (printedParameterNumber - 1) << ';' << std::endl;
				}
			} else if(segment == ":") {
				PrintParameter(indent, parameterCount, false);
				out << indent << "if(p[" << index << "] == '/') {" << std::endl;
				PrintChildren(options, indentLevel, parameterCount, indent);
				out << indent << "} else if(CollectQueries(p + " << index << ")) {" << std::endl;
				PrintFunctionReturn(indent, parameterCount, options);
				out << indent << '}' << std::endl;
			} else {
				if(!segment.empty()) {
					out << indent << "if(";
					PrintCompare();
					out << ") {" << std::endl;
				} else {
					out << indent << '{' << std::endl;
				}
				out << indent << "\tif(p[" << (index + segment.size()) << "] == '/') {" << std::endl;
				PrintChildren(options, indentLevel + 1, parameterCount, indent);
				out << indent << "\t} else if(CollectQueries(p + " << (index + segment.size()) << ")) {" << std::endl;
				PrintFunctionReturn(indent + '\t', parameterCount, options);
				out << indent << "\t}" << std::endl;
				out << indent << '}' << std::endl;
			}
		}

		void PrintChildren(Options const& options, size_t indentLevel, size_t parameterCount, std::string const& indent) const {
			bool isNext = false;
			for(auto const& pair : children) {
				if(isNext && pair.first != ":") {
					out << indent << "\telse" << std::endl;
				} else {
					isNext = true;
				}
				pair.second.Print(options, indentLevel + 1, parameterCount);
			}
		}

		void PrintCompare() const {
			if(segment.size() == 1) {
				out << "p[" << index << "] == '" << segment << "'";
			} else {
				out << "memcmp(p + " << index << ", \"" << segment << "\", " << segment.size() << ") == 0";
			}
		}

		void PrintFunctionReturn(std::string const& indent, size_t parameterCount, Options const& options) const {
			out << indent << "\treturn " << fn << '(';
			for(decltype(parameterCount) i = 0; i < parameterCount; ++i) {
				if(options.wantsStrings) {
					out << "xstring(p" << i << ", " << 'q' << i << ')';
				} else {
					out << 'p' << i << ", " << 'q' << i;
				}
				out << ", ";
			}
			out << "response";
			out << ");" << std::endl;
		}

		size_t PrintParameter(std::string const& indent, size_t& parameterCount, bool wantsSave = true) const {
			if(wantsSave) {
				out << indent << "char const* r" << parameterCount << " = p;" << std::endl;
			}
			out << indent << "char const* p" << parameterCount << " = p + " << index << ';' << std::endl;
			out << indent << "char const* q" << parameterCount << " = CollectParameter(p, " << index << ");" << std::endl;
			return ++parameterCount;
		}
	};
}

SegPrinter::SegPrinter() {}

SegPrinter::~SegPrinter() {}

void SegPrinter::InternalPrint(vector const& requests, Options const& options, std::ostream& out) {
	Node root(out);
	for(auto const& request : requests) {
		root.Add(request);
	}
	root.Collapse();
	root.Print(options);
}
