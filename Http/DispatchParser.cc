#include "pch.h"
#include "DispatchParser.h"

using ptr_t = char const*;

extern void Dispatch(ptr_t begin, ptr_t body, char*& next, ptr_t end, TcpSocket& socket);
extern Task<void> DispatchAsync(ptr_t begin, ptr_t body, char*& next, ptr_t end, AsyncSocket& socket);

std::array<char, 4> pattern = { '\r', '\n', '\r', '\n' };
std::boyer_moore_searcher searcher(pattern.begin(), pattern.end());

DispatchParser::DispatchParser(TcpSocket& socket) {
	// Create an 8K buffer to receive the request.
	constexpr auto bufferSize = 8'000;
	static_assert(bufferSize % sizeof(intptr_t) == 0);
	std::array<intptr_t, bufferSize / sizeof(intptr_t)> buffer;
	auto const begin = reinterpret_cast<char*>(buffer.data()) + pattern.size();
	auto const end = reinterpret_cast<char*>(buffer.data() + buffer.size());

	buffer[0] = 0;
	for (;;) {
		auto m = socket.Receive(begin, end - begin);
		if (m == 0) {
			// There are no more data.
			return;
		}
		if (m < 0) {
			throw std::runtime_error("receive error"); // TODO
		}
		auto next = begin + m;
		auto body = std::search(begin, next, searcher);
		if (body != next) {
			body += pattern.size();
		} else {
			// Read at least the start line and headers.
			body = nullptr;
			auto p = begin;
			do {
				auto n = socket.Receive(next, end - next);
				if (n <= 0) {
					throw std::runtime_error("insufficient data"); // TODO
				}
				next += n;
				auto it = std::search(p, next, searcher);
				if (it != next) {
					body = it + pattern.size();
					break;
				}
				p = next - (pattern.size() - 1);
			} while (next < end);
			if (body == nullptr) {
				throw std::runtime_error("overflow"); // TODO
			}
		}

		// Dispatch to the handler.
		Dispatch(begin, body, next, end, socket);
	}
}

Task<void> DispatchParser::Async(AsyncSocket& socket) {
	// Create a 16K buffer.
	constexpr auto bufferSize = 16'000;
	static_assert(bufferSize % sizeof(intptr_t) == 0);
	std::array<intptr_t, bufferSize / sizeof(intptr_t)> buffer;
	auto const begin = reinterpret_cast<char*>(buffer.data()) + pattern.size();
	auto const end = reinterpret_cast<char*>(buffer.data() + buffer.size());

	buffer[0] = 0;
	for (;;) {
		auto m = co_await socket.Receive(begin, end - begin);
		if (m.second) {
			throw std::runtime_error("receive error"); // TODO
		}
		if (m.first == 0) {
			// There are no more data.
			co_return;
		}
		auto next = begin + m.first;
		auto body = std::search(begin, next, searcher);
		if (body != next) {
			body += pattern.size();
		} else {
			// Read at least the start line and headers.
			body = nullptr;
			auto p = begin;
			do {
				auto n = co_await socket.Receive(next, end - next);
				if (n.second) {
					throw std::runtime_error("receive error"); // TODO
				}
				if (n.first == 0) {
					throw std::runtime_error("insufficient data"); // TODO
				}
				next += n.first;
				auto it = std::search(p, next, searcher);
				if (it != next) {
					body = it + pattern.size();
					break;
				}
				p = next - (pattern.size() - 1);
			} while (next < end);
			if (body == nullptr) {
				throw std::runtime_error("overflow"); // TODO
			}
		}

		// Dispatch to the handler.
		co_await DispatchAsync(begin, body, next, end, socket);
	}
}
