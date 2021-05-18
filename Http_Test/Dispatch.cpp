#include "pch.h"
#include "../Http/Http.h"
#include "Dispatch.h"

using namespace Http_Test::Dispatch;

namespace {
	auto onDispatch = [](ptr_t begin, ptr_t body, ptr_t end, TcpSocket& socket, Response& response) { return end; };
}

std::function<char const* (ptr_t begin, ptr_t body, ptr_t end, TcpSocket& socket, Response& response)> Http_Test::Dispatch::OnDispatch = onDispatch;

char const* Dispatch(ptr_t begin, ptr_t body, ptr_t end, TcpSocket& socket, Response& response) {
	return OnDispatch(begin, body, end, socket, response);
}
