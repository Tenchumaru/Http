#pragma once

#ifdef _WIN32
# include "targetver.h"

# include <WS2tcpip.h>
# include <tchar.h>
# include <crtdbg.h>
#else
# include <fcntl.h>
# include <poll.h>
# include <string.h>
# include <unistd.h>
# include <netdb.h>
# include <cassert>
#endif
#include <openssl/ssl.h>
#include <openssl/err.h>

#include <cstddef>
#include <cstdint>
#include <ctime>
#include <algorithm>
#include <array>
#include <codecvt>
#include <functional>
#include <iostream>
#include <iterator>
#include <locale>
#include <map>
#include <memory>
#include <sstream>
#include <string>
#include <unordered_map>
#include <vector>

#ifdef _WIN32
# define assert _ASSERT
# define close closesocket
# undef errno
# define errno (WSAGetLastError())
# define ioctl ioctlsocket
# undef EALREADY
# define EALREADY WSAEALREADY
# undef EWOULDBLOCK
# define EWOULDBLOCK WSAEWOULDBLOCK
#else
# define _countof(ar) (sizeof(ar) / sizeof((ar)[0]))
# define SOCKET int
# define INVALID_SOCKET (-1)
# define sprintf_s sprintf

using nullptr_t = decltype(nullptr);
#endif

template<typename T>
auto check_(T result, char const* s) {
	if(static_cast<int>(result) < 0) {
		std::cout << s << " error " << errno << std::endl;
		throw std::runtime_error("an error occurred");
	}
	return result;
}

#define check(n) check_(n, #n)
