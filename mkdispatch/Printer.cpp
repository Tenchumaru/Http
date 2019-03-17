#include "pch.h"
#include "Printer.h"

Printer::Printer() {}

Printer::~Printer() {}

void Printer::Print(vector const& requests, Options const& options, std::ostream& out) {
	out << "void Dispatch(char const* p, Response& response) {" << std::endl;
	InternalPrint(requests, options, out);
	out << "\treturn FourZeroFour(response);" << std::endl;
	out << '}' << std::endl;
}
