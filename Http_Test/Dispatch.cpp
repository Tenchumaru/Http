#include "pch.h"
#include "../Http/AsyncSocket.h"
#include "../Http/Http.h"
#include "Dispatch.h"

using namespace Http_Test::Dispatch;

namespace {
	auto onDispatch = [](ptr_t begin, ptr_t body, char*& next, ptr_t end, TcpSocket& socket) {};
	auto onDispatchAsync = [](ptr_t begin, ptr_t body, char*& next, ptr_t end, AsyncSocket& socket) -> Task<void> { co_return; };
}

std::function<void(ptr_t begin, ptr_t body, char*& next, ptr_t end, TcpSocket& socket)> Http_Test::Dispatch::OnDispatch = onDispatch;
std::function<Task<void>(ptr_t begin, ptr_t body, char*& next, ptr_t end, AsyncSocket& socket)> Http_Test::Dispatch::OnDispatchAsync = onDispatchAsync;

void Dispatch(ptr_t begin, ptr_t body, char*& next, ptr_t end, TcpSocket& socket) {
	OnDispatch(begin, body, next, end, socket);
}

Task<void> DispatchAsync(ptr_t begin, ptr_t body, char*& next, ptr_t end, AsyncSocket& socket) {
	return OnDispatchAsync(begin, body, next, end, socket);
}
