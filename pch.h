#pragma once

#ifdef _WIN32
# define WIN32_LEAN_AND_MEAN
# define NOMINMAX
# include "targetver.h"

# include <WS2tcpip.h>
# include <tchar.h>
# include <crtdbg.h>
#else
# include <arpa/inet.h>
# include <fcntl.h>
# include <poll.h>
# include <string.h>
# include <unistd.h>
# include <netdb.h>
# include <cassert>
#endif

#include <cstddef>
#include <cstdint>
#include <ctime>
#include <algorithm>
#include <array>
#include <atomic>
#include <chrono>
#include <codecvt>
#include <fstream>
#include <functional>
#include <future>
#include <iomanip>
#include <iostream>
#include <iterator>
#include <locale>
#include <map>
#include <memory>
#include <optional>
#include <set>
#include <sstream>
#include <string>
#include <unordered_map>
#include <variant>
#include <vector>

#ifdef _WIN32
# define assert _ASSERT
# define close closesocket
# undef errno
# define errno (WSAGetLastError())
# define ioctl ioctlsocket
# undef EALREADY
# define EALREADY WSAEALREADY
# undef ECONNRESET
# define ECONNRESET WSAECONNRESET
# undef EINPROGRESS
# define EINPROGRESS WSAEINPROGRESS
# undef EINVAL
# define EINVAL WSAEINVAL
# undef EWOULDBLOCK
# define EWOULDBLOCK WSAEWOULDBLOCK
#else
# include <cmath>

# define _countof(ar) (sizeof(ar) / sizeof((ar)[0]))
# define SOCKET int
# define INVALID_SOCKET (-1)
# define WINAPI
# define sprintf_s sprintf
# define _strnicmp strncasecmp

union SOCKADDR_INET {
	sockaddr_in Ipv4;
	sockaddr_in6 Ipv6;
	sa_family_t si_family;
};

inline error_t memcpy_s(void* p, size_t n, void const* q, size_t m) {
	if (n < m) {
		assert(m >= n);
		return ENOSPC;
	}
	memcpy(p, q, m);
	return 0;
}

inline error_t memmove_s(void* p, size_t n, void const* q, size_t m) {
	if (n < m) {
		assert(m >= n);
		return ENOSPC;
	}
	memmove(p, q, m);
	return 0;
}
#endif

template<typename T>
auto check_(T result, char const* s) {
	if (static_cast<int>(result) < 0) {
		std::cerr << s << " error " << errno << std::endl;
		throw std::runtime_error("an error occurred");
	}
	return result;
}

#define check(n) check_(n, #n)
