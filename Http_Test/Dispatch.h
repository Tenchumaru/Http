#pragma once

namespace Http_Test {
	namespace Dispatch {
		using ptr_t = char const*;

		extern std::function<void(ptr_t begin, ptr_t body, char*& next, ptr_t end, TcpSocket& client)> OnDispatch;

		void Initialize();
	}
}
