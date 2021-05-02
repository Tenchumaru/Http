#include "pch.h"
#include "Printer.h"

Printer::Printer() {}

Printer::~Printer() {}

void Printer::Print(vector const& requests, Options const& options, std::ostream& out) {
	out << "char const* Dispatch(char const* p, char const* body, char* next, char const* end, TcpSocket& socket, Response& response) {" << std::endl;
	out << "\tbody, next, end, socket;" << std::endl;
	InternalPrint(requests, options, out);
	out << "\tFourZeroFour(response);" << std::endl;
	out << "\treturn next;" << std::endl;
	out << '}' << std::endl;
}
