#pragma once

#ifdef _WIN32
# define WIN32_LEAN_AND_MEAN
# include "targetver.h"
# include <Windows.h>
#else
# include <cmath>
#endif

#include <algorithm>
#include <codecvt>
#include <iomanip>
#include <memory>
#include <ostream>
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
