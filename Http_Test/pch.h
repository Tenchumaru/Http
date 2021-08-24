#pragma once

#include "../pch.h"
#include <openssl/ssl.h>
#include <openssl/err.h>
#ifdef _WIN32
# include <CppUnitTest.h>
#else
# include <Fiber.h>

# define TEST_CLASS(name) class name
# define TEST_CLASS_CLEANUP(name) static void name()
# define TEST_CLASS_INITIALIZE(name) static void name()
# define TEST_METHOD(name) void name()
# define TEST_METHOD_INITIALIZE(name) void name()
# define TEST_MODULE_INITIALIZE(name) void name()
# define WSAAPI
# define WSAEWOULDBLOCK EWOULDBLOCK
# define strncpy_s(a,b,c,d) strncpy(a, c, d)
# define closesocket close

inline void WSASetLastError(int errorCode) { errno = errorCode; }

using SOCKET = int;

constexpr int SOCKET_ERROR = -1;

namespace Microsoft {
	namespace VisualStudio {
		namespace CppUnitTestFramework {
			template<typename T>
			inline std::string AsString(T const& t) {
				return std::string(t);
			}

			template<>
			inline std::string AsString(bool const& t) {
				return t ? "true" : "false";
			}

			template<>
			inline std::string AsString(double const& t) {
				return std::to_string(t);
			}

			template<>
			inline std::string AsString(int const& t) {
				return std::to_string(t);
			}

			template<>
			inline std::string AsString(unsigned const& t) {
				return std::to_string(t);
			}

			template<>
			inline std::string AsString(unsigned long const& t) {
				return std::to_string(t);
			}

			template<>
			inline std::string AsString(void* const& t) {
				std::stringstream ss;
				ss << t;
				return ss.str();
			}

			template<>
			inline std::string AsString(void const* const& t) {
				std::stringstream ss;
				ss << t;
				return ss.str();
			}

			template<typename T>
			inline std::string AsString(std::vector<T> const& v) {
				std::stringstream ss;
				for (auto& t : v) {
					ss << ", " << AsString(t);
				}
				ss << '}';
				auto s = ss.str();
				s[0] = '{';
				return s;
			}

			class Assert {
			public:
				template<typename T>
				static void AreEqual(T const& expected, T const& actual, char const* s) {
					if (expected != actual) {
						std::cerr << "\tequality assertion failure in " << s << std::endl;
						std::cerr << "\texpected: \"" << AsString(expected) << "\", actual: \"" << AsString(actual) << '"' << std::endl;
					}
				}

				static void AreEqual(char const* expected, char const* actual, char const* s) {
					if (strcmp(expected, actual)) {
						std::cerr << "\tequality assertion failure in " << s << std::endl;
						std::cerr << "\texpected: \"" << expected << "\", actual: \"" << actual << '"' << std::endl;
					}
				}
# define AreEqual(a,b) AreEqual(a, b, __func__)

				template<typename T>
				static void AreNotEqual(T const& expected, T const& actual, char const* s) {
					if (expected == actual) {
						std::cerr << "\tinequality assertion failure in " << s << std::endl;
						std::cerr << "\tnot expected: \"" << AsString(expected) << '"' << std::endl;
					}
				}
# define AreNotEqual(a,b) AreNotEqual(a, b, __func__)

				static void IsFalse(bool b, char const* s) {
					if (b) {
						std::cerr << "\tfalsity assertion failure in " << s << std::endl;
					}
				}
# define IsFalse(v) IsFalse(v, __func__)

				static void IsNotNull(void const* p, char const* s) {
					if (!p) {
						std::cerr << "\tnot null assertion failure in " << s << std::endl;
					}
				}
# define IsNotNull(v) IsNotNull(v, __func__)

				static void IsNull(void const* p, char const* s) {
					if (p) {
						std::cerr << "\tnull assertion failure in " << s << std::endl;
					}
				}
# define IsNull(v) IsNull(v, __func__)

				static void IsTrue(bool b, char const* s) {
					if (!b) {
						std::cerr << "\ttruth assertion failure in " << s << std::endl;
					}
				}
# define IsTrue(v) IsTrue(v, __func__)

				template<typename E, typename F>
				static void ExpectException(F f, wchar_t const* message) {
					try {
						f();
						std::cerr << "\tno";
					} catch (E) {
						return;
					} catch (...) {
						std::cerr << "\tunexpected";
					}
					std::cerr << " exception";
					if (message) {
						std::cerr << ":  " << message;
					}
					std::cerr << std::endl;
				}
			};
		}
	}
}
#endif
