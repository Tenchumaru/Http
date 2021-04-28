#include "pch.h"
#include "Printer.h"

Printer::Printer() {}

Printer::~Printer() {}

void Printer::Print(vector const& requests, Options const& options, std::ostream& out) {
	if (options.wantsAsynchronous) {
		out << "void Dispatch(char const*, char const*, char*&, char const*, TcpSocket&) {}" << std::endl;
		out << "Task<void> DispatchAsync(char const* p, char const* body, char*& next, char const* end, AsyncSocket& socket) {" << std::endl;
	} else {
		out << "Task<void> DispatchAsync(char const*, char const*, char*&, char const*, AsyncSocket&) { throw std::runtime_error(\"not implemented\"); }" << std::endl;
		out << "void Dispatch(char const* p, char const* body, char*& next, char const* end, TcpSocket& socket) {" << std::endl;
	}
	out << "\tbody, next, end;" << std::endl;
	out << "\tstd::array<char, 0x1000> buffer;" << std::endl;
	if (options.wantsAsynchronous) {
		out << "\tClosableAsyncResponse response(socket, buffer.data(), buffer.data() + buffer.size());" << std::endl;
	} else {
		out << "\tClosableResponse response(socket, buffer.data(), buffer.data() + buffer.size());" << std::endl;
	}
	InternalPrint(requests, options, out);
	if (options.wantsAsynchronous) {
		out << "\tco_await FourZeroFourAsync(response);" << std::endl;
		out << "\tco_return;" << std::endl;
	} else {
		out << "\treturn FourZeroFour(response);" << std::endl;
	}
	out << '}' << std::endl;
}
