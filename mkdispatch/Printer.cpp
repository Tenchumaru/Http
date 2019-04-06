#include "pch.h"
#include "Printer.h"

Printer::Printer() {}

Printer::~Printer() {}

void Printer::Print(vector const& requests, Options const& options, std::ostream& out) {
	out << "void Dispatch(char const* p, char const* body, char*& next, char const* end, TcpSocket& client) {" << std::endl;
	out << "\tbody, next, end;" << std::endl;
	out << "\tstd::array<char, 0x1000> x;" << std::endl;
	out << "\tClosableResponse response(client, x.data(), x.data() + x.size());" << std::endl;
	InternalPrint(requests, options, out);
	out << "\treturn FourZeroFour(response);" << std::endl;
	out << '}' << std::endl;
}
