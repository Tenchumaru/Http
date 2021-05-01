#include "pch.h"
#include "Printer.h"

Printer::Printer() {}

Printer::~Printer() {}

void Printer::Print(vector const& requests, Options const& options, std::ostream& out) {
	if (options.wantsAsynchronous) {
		out << "void Dispatch(char const*, char const*, char*&, char const*, TcpSocket&, Response&) {}" << std::endl;
		out << "Task<void> DispatchAsync(char const* p, char const* body, char*& next, char const* end, AsyncSocket& socket, AsyncResponse& response) {" << std::endl;
	} else {
		out << "Task<void> DispatchAsync(char const*, char const*, char*&, char const*, AsyncSocket&, AsyncResponse&) { throw std::runtime_error(\"not implemented\"); }" << std::endl;
		out << "void Dispatch(char const* p, char const* body, char*& next, char const* end, TcpSocket& socket, Response& response) {" << std::endl;
	}
	out << "\tbody, next, end, socket;" << std::endl;
	InternalPrint(requests, options, out);
	if (options.wantsAsynchronous) {
		out << "\tco_await FourZeroFourAsync(response);" << std::endl;
		out << "\tco_return;" << std::endl;
	} else {
		out << "\treturn FourZeroFour(response);" << std::endl;
	}
	out << '}' << std::endl;
}
