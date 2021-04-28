#pragma once

namespace Http_Test {
	namespace Dispatch {
		using ptr_t = char const*;

		extern std::function<void(ptr_t begin, ptr_t body, char*& next, ptr_t end, TcpSocket& socket)> OnDispatch;
		extern std::function<Task<void>(ptr_t begin, ptr_t body, char*& next, ptr_t end, AsyncSocket& socket)> OnDispatchAsync;

		void Initialize();
	}
}
