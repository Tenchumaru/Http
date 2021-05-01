#include "pch.h"
#include "Printer.h"

Printer::Printer() {}

Printer::~Printer() {}

void Printer::Print(vector const& requests, Options const& options, std::ostream& out) {
	out << "void Dispatch(char const* p, char const* body, char*& next, char const* end, TcpSocket& socket) {" << std::endl;
	out << "\tbody, next, end;" << std::endl;
	out << "\tstd::array<char, 0x1000> buffer;" << std::endl;
	out << "\tClosableResponse response(socket, buffer.data(), buffer.data() + buffer.size());" << std::endl;
	InternalPrint(requests, options, out);
	out << "\treturn FourZeroFour(response);" << std::endl;
	out << '}' << std::endl;
}
