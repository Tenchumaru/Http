#include "pch.h"
#include "DispatchParser.h"
#include "ClosableResponse.h"
#include "ClosableAsyncResponse.h"

using ptr_t = char const*;

extern void Dispatch(ptr_t begin, ptr_t body, char*& next, ptr_t end, TcpSocket& socket, Response& response);
extern Task<void> DispatchAsync(ptr_t begin, ptr_t body, char*& next, ptr_t end, AsyncSocket& socket, AsyncResponse& response);

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
		// Create an 8K buffer to send the response.
		// TODO:  consider creating a flushable response to allow for
		// content larger than this buffer.
		static_assert(bufferSize % sizeof(intptr_t) == 0);
		std::array<intptr_t, bufferSize / sizeof(intptr_t)> responseBuffer;
		auto* const p = reinterpret_cast<char*>(responseBuffer.data()) + pattern.size();
		auto* const q = reinterpret_cast<char*>(responseBuffer.data() + responseBuffer.size());
		ClosableResponse response(socket, p, q);

		auto const m = socket.Receive(begin, end - begin);
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
			auto* here = begin;
			do {
				auto const n = socket.Receive(next, end - next);
				if (n <= 0) {
					throw std::runtime_error("insufficient data"); // TODO
				}
				next += n;
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
		Dispatch(begin, body, next, end, socket, response);
		response.Close();
	}
}

Task<void> DispatchParser::Async(AsyncSocket& socket) {
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
		// TODO:  consider creating a flushable response to allow for
		// content larger than this buffer.
		static_assert(bufferSize % sizeof(intptr_t) == 0);
		std::array<intptr_t, bufferSize / sizeof(intptr_t)> responseBuffer;
		auto* const p = reinterpret_cast<char*>(responseBuffer.data()) + pattern.size();
		auto* const q = reinterpret_cast<char*>(responseBuffer.data() + responseBuffer.size());
		ClosableAsyncResponse response(socket, p, q);

		auto const m = co_await socket.Receive(begin, end - begin);
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
			body = nullptr;
			auto* here = begin;
			do {
				auto const n = co_await socket.Receive(next, end - next);
				if (n.second) {
					throw std::runtime_error("receive error"); // TODO
				}
				if (n.first == 0) {
					throw std::runtime_error("insufficient data"); // TODO
				}
				next += n.first;
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
		co_await DispatchAsync(begin, body, next, end, socket, response);
		co_await response.Close();
	}
}
