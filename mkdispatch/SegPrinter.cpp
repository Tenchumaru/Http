#include "pch.h"
#include "SegPrinter.h"

namespace {
	struct Segment {
		using ptr = std::shared_ptr<Segment>;
		using vector = std::vector<Segment::ptr>;

		std::string value, fn;
		ptr next;
		size_t index, parameter;

		Segment(Printer::Request const& request, size_t index, size_t parameter) : index(index), parameter(parameter) {
			auto i = request.line.find('/', index);
			if(i == request.line.npos) {
				value = request.line.substr(index);
				fn = request.fn;
			} else {
				value = request.line.substr(index, i - index);
				if(value == ":") {
					++parameter;
				}
				next = std::make_shared<Segment>(request, i + 1, parameter);
			}
		}
	};

	class Node {
	public:
		Node(Segment::vector const& segments, std::string const& fn, size_t index, size_t parameter) : fn(fn), index(index), parameter(parameter) {
			std::map<std::string, Segment::vector> partitions;
			std::map<std::string, std::string> functions;
			for(auto const& segment : segments) {
				auto next = segment->next;
				auto& v = partitions[segment->value];
				if(next) {
					v.push_back(next);
				}
				if(!segment->fn.empty()) {
					if(!functions.insert({ segment->value, segment->fn }).second) {
						std::cerr << "warning: duplicate function \"" << segment->fn << '"' << std::endl;
					}
				}
			}
			for(auto const& partition : partitions) {
				auto it = functions.find(partition.first);
				if(partition.first == ":") {
					++parameter;
				}
				auto childFn = it == functions.cend() ? std::string() : it->second;
				size_t childIndex = index + partition.first.size() + 1;
				children.insert({ partition.first, Node(partition.second, childFn, childIndex, parameter) });
			}
		}

		void Print(size_t indentLevel) const {
			std::string indent(indentLevel, '\t');
			std::for_each(children.crbegin(), children.crend(), [this, indent, indentLevel](map::value_type const& pair) {
				auto const& name = pair.first;
				auto const& child = pair.second;
				if(name == ":") {
					std::cout << indent << "char const* p" << parameter << " = CollectParameter(p, " << index << ");" << std::endl;
					std::cout << indent << "char const* q" << parameter << " = p + " << (index + 1) << ';' << std::endl;
					std::cout << indent << "if(p[" << (index + 1) << "] == '/') {" << std::endl;
				} else {
					if(!child.children.empty()) {
						std::cout << indent << "if(memcmp(p + " << index << ", \"" << name << "/\", " << (name.size() + 1) << ") {" << std::endl;
					} else if(name.size() == 1) {
						std::cout << indent << "if(p[" << index << "] == '" << name << "') {" << std::endl;
					} else {
						std::cout << indent << "if(memcmp(p + " << index << ", \"" << name << "\", " << name.size() << ") {" << std::endl;
					}
				}
				child.Print(indentLevel + 1);
				std::cout << indent << '}' << std::endl;
			});
			if(!fn.empty()) {
				std::cout << indent << "return " << fn << '(';
				for(size_t i = 0; i < parameter; ++i) {
					std::cout << 'p' << i << ", " << 'q' << i;
					if(i + 1 < parameter) {
						std::cout << ", ";
					}
				}
				std::cout << "); " << std::endl;
			}
		}

	private:
		using map = std::map<std::string, Node>;

		map children;
		std::string fn;
		size_t index, parameter;
	};
}

SegPrinter::SegPrinter() {}

SegPrinter::~SegPrinter() {}

void SegPrinter::InternalPrint(vector const& requests, Options const& options) {
	options;

	// Go through the requests in reverse.  This implementation favors requests
	// processed earlier over those processed later.  TODO:  perhaps iteration
	// order here doesn't matter.  Perhaps the iteratior order of the maps in
	// the Node matters.
	Segment::vector segments;
	std::transform(requests.crbegin(), requests.crend(), std::back_inserter(segments), [](Request const& request) {
		return std::make_shared<Segment>(request, 0, 0);
	});

	Node root(segments, std::string(), 0, 0);
	root.Print(1);
}
