#include "pch.h"
#include "../Http/Http.h"
#include "Dispatch.h"

using namespace Http_Test::Dispatch;

namespace {
	auto onDispatch = [](ptr_t begin, ptr_t body, char*& next, ptr_t end, TcpSocket& socket) {};
}

std::function<void(ptr_t begin, ptr_t body, char*& next, ptr_t end, TcpSocket& socket)> Http_Test::Dispatch::OnDispatch = onDispatch;

void Dispatch(ptr_t begin, ptr_t body, char*& next, ptr_t end, TcpSocket& socket) {
	OnDispatch(begin, body, next, end, socket);
}
