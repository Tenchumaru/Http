#include "stdafx.h"
#include "DispatchParser.h"

using ptr_t = char const*;

class Body {
public:
	Body(ptr_t begin, ptr_t& next, TcpSocket& client);
	Body(Body const&) = delete;
	Body(Body&&) = default;
	Body& operator=(Body const&) = delete;
	Body& operator=(Body&&) = default;
	~Body() = default;

private:
	ptr_t begin;
	ptr_t& next;
	TcpSocket& client;
};

extern void Dispatch(ptr_t begin, ptr_t body, char*& next, ptr_t end, TcpSocket& client);

Body::Body(ptr_t begin, ptr_t& next, TcpSocket& client) : begin(begin), next(next), client(client) {}

std::array<char, 4> pattern = { '\r', '\n', '\r', '\n' };
std::boyer_moore_searcher searcher(pattern.begin(), pattern.end());

DispatchParser::DispatchParser(TcpSocket& client) : client(client) {
	// Create a 16K buffer.
	constexpr auto bufferSize = 16 * 1024;
	static_assert(bufferSize % sizeof(intptr_t) == 0);
	std::array<intptr_t, bufferSize / sizeof(intptr_t)> buffer;
	auto const begin = reinterpret_cast<char*>(buffer.data()) + pattern.size();
	auto const end = reinterpret_cast<char*>(buffer.data() + buffer.size());

	buffer[0] = 0;
	for(;;) {
		auto m = client.Receive(begin, end - begin);
		if(m == 0) {
			// There are no more data.
			return;
		}
		auto next = begin + m;
		auto body = std::search(begin, next, searcher);
		if(body != next) {
			body += pattern.size();
		} else {
			// Read at least the start line and headers.
			body = nullptr;
			auto p = begin;
			do {
				auto n = client.Receive(next, end - next);
				if(n <= 0) {
					throw std::runtime_error("insufficient data"); // TODO
				}
				next += n;
				auto it = std::search(p, next, searcher);
				if(it != next) {
					body = it + pattern.size();
					break;
				}
				p = next - (pattern.size() - 1);
			} while(next < end);
			if(body == nullptr) {
				throw std::runtime_error("overflow"); // TODO
			}
		}

		// Dispatch to the handler.
		Dispatch(begin, body, next, end, client);
	}
}

DispatchParser::~DispatchParser() {}
