#pragma once

#ifdef _WIN32
# include "targetver.h"

# include <WS2tcpip.h>
# include <tchar.h>
#else
# include <cmath>
# define SOCKET int
#endif

#include <array>
#include <functional>
#include <iostream>
#include <locale>
#include <map>
#include <sstream>
#include <string>
#include <unordered_map>
#include <vector>

#ifndef _WIN32
using std::nullptr_t;
using std::isinf;

namespace std {
	template<class _Ty>
	constexpr bool is_integral_v = is_integral<_Ty>::value;
	template<class T, class U>
	constexpr bool is_same_v = std::is_same<T, U>::value;
}
#endif
