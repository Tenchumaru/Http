#include "pch.h"
#include "Printer.h"

Printer::Printer() {}

Printer::~Printer() {}

void Printer::Print(vector const& requests, Options const& options) {
	std::cout << "void Dispatch(char const* p) {" << std::endl;
	InternalPrint(requests, options);
	std::cout << "\treturn FourZeroFour();" << std::endl;
	std::cout << '}' << std::endl;
}
