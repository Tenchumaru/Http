#include "pch.h"
#include "../Http/AsyncSocket.h"
#include "../Http/AsyncResponse.h"
#include "../Http/Http.h"
#include "Dispatch.h"

using namespace Http_Test::Dispatch;

namespace {
	auto onDispatch = [](ptr_t, ptr_t, char*&, ptr_t, TcpSocket&, Response&) {};
	auto onDispatchAsync = [](ptr_t, ptr_t, char*&, ptr_t, AsyncSocket&, AsyncResponse&) -> Task<void> { co_return; };
}

std::function<void(ptr_t begin, ptr_t body, char*& next, ptr_t end, TcpSocket& socket, Response& response)> Http_Test::Dispatch::OnDispatch = onDispatch;
std::function<Task<void>(ptr_t begin, ptr_t body, char*& next, ptr_t end, AsyncSocket& socket, AsyncResponse& response)> Http_Test::Dispatch::OnDispatchAsync = onDispatchAsync;

void Dispatch(ptr_t begin, ptr_t body, char*& next, ptr_t end, TcpSocket& socket, Response& response) {
	OnDispatch(begin, body, next, end, socket, response);
}

Task<void> DispatchAsync(ptr_t begin, ptr_t body, char*& next, ptr_t end, AsyncSocket& socket, AsyncResponse& response) {
	return OnDispatchAsync(begin, body, next, end, socket, response);
}
