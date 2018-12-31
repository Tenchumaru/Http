#include "pch.h"
#include "MyPrinter.h"

MyPrinter::MyPrinter() {}

MyPrinter::~MyPrinter() {}

void MyPrinter::InternalParse(std::string const& line, std::string const& fn, size_t index, Node& node) {
	// If the node is empty (i.e., has neither nodes nor a prefix), set its
	// line and prefix.
	auto& prefix = node.prefix;
	auto suffix = line.substr(index);
	if(node.nodes.empty() && prefix.empty()) {
		node.fn = fn;
		std::swap(prefix, suffix);
		node.index = index;
	} else {
		// Find the index of the character at which the suffix and prefix diverge.
		auto pair = std::mismatch(prefix.cbegin(), prefix.cend(), suffix.cbegin(), suffix.cend());

		// If the suffix does not start with the prefix, split the node
		// into two nodes, one with the common part of the prefix and
		// another with the rest of it.  The latter node will contain the
		// current node's nodes and the former will contain the latter.
		if(pair.first != prefix.cend()) {
			size_t divergenceIndex = pair.first - prefix.cbegin();
			/*
			before:
			node {
				nodes: {N},
				line: S + A + "bC",
				prefix: A + "bC",
				index: S.size(),
			}

			after adding suffix A + "xY" of line S + A + "xY":
			node {
				nodes: {
					'b': {
						nodes: {N},
						line: S + A + "bC",
						prefix: "C",
						index: S.size() + A.size() + 1,
					},
					'x': {
						nodes: {},
						line: S + A + "xY",
						prefix: "Y",
						index: S.size() + A.size() + 1,
					},
				},
				line: "",
				prefix: A,
				index: S.size(),
			}
			*/
			node.nodes = { { *pair.first, { node.nodes, node.fn, prefix.substr(divergenceIndex + 1), node.index + divergenceIndex + 1 } } };
			node.fn.clear();
			prefix.erase(divergenceIndex);
			index += divergenceIndex;
			suffix.erase(0, divergenceIndex);
		} else if(prefix.size()) {
			// The suffix starts with the prefix.  Remove the latter from
			// the former and adjust the index.
			suffix.erase(0, prefix.size());
			index += prefix.size();
		}

		// If the suffix is empty, replace this node's line.
		if(suffix.empty()) {
			if(!node.fn.empty()) {
				// Warn about replacement.
				std::cout << "warning: replacing \"" << node.fn << "\" with \"" << fn << '"' << std::endl;
			}
			node.fn = fn;
		} else {
			// Otherwise, recurse into the child node.
			auto& child = node.nodes[suffix[0]];
			InternalParse(line, fn, index + 1, child);
		}
	}
}

void MyPrinter::InternalPrint(vector const& requests, Options const& options) {
	Node root = Parse(requests);
	std::cout << "/* ";
	Dump(root, 0);
	std::cout << "*/" << std::endl;
	Print(root, 1, 0, options);
}

void MyPrinter::ReplaceParameter(Node& node) {
	/*
	before:
	node {
		nodes: {N},
		line: S + A + flag + B,
		prefix: A + flag + B,
		index: S.size(),
	}

	after:
	node {
		nodes: {
			flag: {
				nodes: {N},
				line: S + A + flag + B,
				prefix: B,
				index: S.size() + A.size() + 1,
			},
		},
		line: "",
		prefix: A,
		index: S.size(),
	}
	*/
	auto i = node.prefix.find(flag);
	if(i != std::string::npos) {
		node.nodes = { { flag, { node.nodes, node.fn, node.prefix.substr(i + 1), node.index + i + 1 } } };
		node.fn.clear();
		node.prefix.erase(i);
	}
	std::for_each(node.nodes.begin(), node.nodes.end(), [this](Node::map::value_type& pair) { ReplaceParameter(pair.second); });
}

MyPrinter::Node MyPrinter::Parse(vector const& requests) {
	// Add each line, with parameter names removed, to the node structure.
	Node root;
	std::for_each(requests.cbegin(), requests.cend(), [this, &root](Request const& request) {
		std::string processedLine;
		std::string::size_type i = 0, j;
		while(j = request.line.find(':', i), j != std::string::npos) {
			processedLine.append(request.line.cbegin() + i, request.line.cbegin() + j);
			processedLine += flag;
			i = request.line.find('/', j);
			if(i == std::string::npos) {
				i = request.line.size();
			}
		}
		processedLine.append(request.line.substr(i));
		InternalParse(processedLine, request.fn, 0, root);
	});

	// Replace any parameters in prefixes with an additional node.  This
	// will allow the printing logic to represent them properly.
	ReplaceParameter(root);
	return root;
}

void MyPrinter::Dump(Node const& node, size_t level /*= 0*/) {
#ifdef _DEBUG
	auto const& prefix = node.prefix;
	if(!prefix.empty()) {
		std::cout << '"' << prefix << "\" ";
	}
	std::cout << '(' << node.index << "): ";
	if(!node.fn.empty()) {
		std::cout << '"' << node.fn;
		if(node.nodes.empty()) {
			std::cout << '"' << std::endl;
		} else {
			std::cout << "\" ";
		}
	}
	if(!node.nodes.empty()) {
		std::cout << '{' << std::endl;
		std::vector<std::pair<Node::map::key_type, Node::map::mapped_type>> v(node.nodes.cbegin(), node.nodes.cend());
		std::sort(v.begin(), v.end(), [](auto const& left, auto const& right) {
			return left.first < right.first;
		});
		std::for_each(v.crbegin(), v.crend(), [this, level = level + 1](auto const& pair) {
			std::cout << std::string(level, ' ') << pair.first << ": ";
			Dump(pair.second, level);
		});
		std::cout << std::string(level, ' ') << '}' << std::endl;
	}
#else
	node, level;
#endif
}

void MyPrinter::PrintCompare(Node::map::value_type const& pair, size_t level, size_t parameterIndex, Options const& options) {
	if(!pair.second.prefix.empty()) {
		auto tabs = std::string(level, '\t');
		std::cout << tabs;
		if(pair.second.prefix.size() == 1) {
			std::cout << "if(p[" << pair.second.index << "] == '" << pair.second.prefix << "') {" << std::endl;
		} else {
			std::cout << "if(memcmp(p + " << pair.second.index << ", \"";
			std::cout << pair.second.prefix << "\", " << pair.second.prefix.size() << ") == 0) {" << std::endl;
		}
		Print(pair.second, level + 1, parameterIndex, options);
		std::cout << tabs << '}' << std::endl;
	} else {
		Print(pair.second, level, parameterIndex, options);
	}
}

void MyPrinter::Print(Node const& node, size_t level, size_t parameterIndex, Options const& options) {
	auto tabs = std::string(level, '\t');
	if(!node.nodes.empty()) {
		if(node.nodes.size() >= 2 || node.nodes.cbegin()->first != flag) {
			std::cout << tabs << "switch(p[" << (node.index + node.prefix.size()) << "]) {" << std::endl;
			std::for_each(node.nodes.crbegin(), node.nodes.crend(), [this, level, &options, parameterIndex, &tabs](Node::map::value_type const& pair) {
				if(pair.first == flag) {
					return;
				}
				std::cout << tabs << "case '" << pair.first << "':" << std::endl;
				PrintCompare(pair, level + 1, parameterIndex, options);
				std::cout << tabs << "\tbreak;" << std::endl;
			});
			std::cout << tabs << '}' << std::endl;
		}
		auto const& pair = *node.nodes.cbegin();
		if(pair.first == flag) {
			std::cout << tabs << "char const* p" << parameterIndex << ';' << std::endl;
			std::cout << tabs << "char* q" << parameterIndex << ';' << std::endl;
			std::cout << tabs << "std::tie(p" << parameterIndex << ", q" << parameterIndex << ") = CollectParameter(p, ";
			std::cout << pair.second.index << ");" << std::endl;
			PrintCompare(pair, level, parameterIndex + 1, options);
		}
	}
	if(!node.fn.empty()) {
		std::cout << tabs << "if(IsEndOfPath(p[" << (node.index + node.prefix.size()) << "])) {" << std::endl;
		std::ostringstream parameters;
		for(size_t i = 0; i < parameterIndex; ++i) {
			if(options.wantsStrings) {
				parameters << "xstring(p" << i << ", q" << i << ')';
				if(i + 1 < parameterIndex) {
					parameters << ", ";
				}
			} else {
				std::cout << tabs << "\t*q" << i << " = '\\0';" << std::endl;
				parameters << 'p' << i;
				if(i + 1 < parameterIndex) {
					parameters << ", ";
				}
			}
		}
		std::cout << tabs << "\treturn " << node.fn << '(' << parameters.str() << ");" << std::endl;
		std::cout << tabs << '}' << std::endl;
	}
}
