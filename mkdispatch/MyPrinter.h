#pragma once

#include "Printer.h"

class MyPrinter : public Printer {
public:
	MyPrinter();
	~MyPrinter();

private:
	struct Node {
		std::map<char, Node> nodes;
		std::string fn;
		std::string prefix;
		size_t index;

		using map = decltype(nodes);
	};

	static char constexpr flag = '\x1e';

	void Dump(Node const& node, size_t level = 0);
	void InternalParse(std::string const& line, std::string const& fn, size_t index, Node& node);
	void InternalPrint(vector const& requests, Options const& options) override;
	Node Parse(vector const& requests);
	void Print(Node const& node, size_t level, size_t parameterIndex, Options const& options);
	void PrintCompare(Node::map::value_type const& pair, size_t level, size_t parameterIndex, Options const& options);
	void ReplaceParameter(Node& node);
};
