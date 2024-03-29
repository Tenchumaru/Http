#pragma once

#include "Options.h"

class Printer {
public:
	struct Request {
		std::string line;
		std::string fn;
		std::string queriesFn;
		std::string headersFn;
	};

	using vector = std::vector<Request>;

	Printer();
	virtual ~Printer() = 0;
	void Print(vector const& requests, Options const& options, std::ostream& out);

private:
	virtual void InternalPrint(vector const& requests, Options const& options, std::ostream& out) = 0;
};
