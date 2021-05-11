#pragma once

namespace Http_Test {
	namespace Dispatch {
		using ptr_t = char const*;

		extern std::function<char const* (ptr_t begin, ptr_t body, ptr_t end, TcpSocket& socket, Response& response)> OnDispatch;
	}
}
