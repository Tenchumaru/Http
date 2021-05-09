#include "pch.h"
#include "AsyncSocket.h"
#include "ClosableAsyncResponse.h"
#include "StaticHttpServer.h"

extern Task<void> DispatchAsync(char const* begin, char const* body, char*& next, char const* end, AsyncSocket& socket, AsyncResponse& response);

std::array<char, 4> pattern = { '\r', '\n', '\r', '\n' };
std::boyer_moore_searcher searcher(pattern.begin(), pattern.end());

Task<void> StaticHttpServer::Handle(std::unique_ptr<AsyncSocket> socket) {
	try {
		// Create an 8K buffer to receive the request.
		constexpr auto bufferSize = 8'000;
		static_assert(bufferSize % sizeof(intptr_t) == 0);
		std::array<intptr_t, bufferSize / sizeof(intptr_t)> buffer;
		auto* const begin = reinterpret_cast<char*>(buffer.data()) + pattern.size();
		auto* const end = reinterpret_cast<char*>(buffer.data() + buffer.size());

		// Read at least the start line and headers.
		buffer[0] = 0;
		for (;;) {
			// Create an 8K buffer to send the response.
			std::array<intptr_t, bufferSize / sizeof(intptr_t)> responseBuffer;
			auto* const p = reinterpret_cast<char*>(responseBuffer.data()) + pattern.size();
			auto* const q = reinterpret_cast<char*>(responseBuffer.data() + responseBuffer.size());
			ClosableAsyncResponse response(*socket, p, q);

			auto result = co_await socket->Receive(begin, end - begin);
			if (result.second) {
				throw std::runtime_error("receive error"); // TODO
			}
			if (result.first == 0) {
				// There are no more data.
				co_return;
			}
			auto next = begin + result.first;
			auto body = std::search(begin, next, searcher);
			if (body != next) {
				body += pattern.size();
			} else {
				body = nullptr;
				auto* here = begin;
				do {
					result = co_await socket->Receive(next, end - next);
					if (result.second) {
						throw std::runtime_error("receive error"); // TODO
					}
					if (result.first == 0) {
						throw std::runtime_error("insufficient data"); // TODO
					}
					next += result.first;
					auto const it = std::search(here, next, searcher);
					if (it != next) {
						body = it + pattern.size();
						break;
					}
					here = next - (pattern.size() - 1);
				} while (next < end);
				if (body == nullptr) {
					throw std::runtime_error("overflow"); // TODO
				}
			}

			// Dispatch to the handler.
			co_await DispatchAsync(begin, body, next, end, *socket, response);
			co_await response.Close();
		}
	} catch (std::exception const& /*ex*/) {
		// TODO:  Handle the exception.
	}
}
