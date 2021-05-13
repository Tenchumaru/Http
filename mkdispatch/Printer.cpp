#include "pch.h"
#include "Printer.h"

Printer::Printer() {}

Printer::~Printer() {}

void Printer::Print(vector const& requests, Options const& options, std::ostream& out) {
	out << "// This is an auto-generated file.  Do not edit." << std::endl;
	out << "char const* Dispatch(char const* p, char const* body, char const* end, TcpSocket& socket, Response& response) {" << std::endl;
	out << "\tbody, end, socket;" << std::endl;
	InternalPrint(requests, options, out);
	out << "\tFourExEx(response, StatusLines::NotFound);" << std::endl;
	out << "\treturn end;" << std::endl;
	out << '}' << std::endl;
}
