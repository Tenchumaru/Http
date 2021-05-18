#include "pch.h"
#include "../Http/HeaderBase.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace Http_Test {
	TEST_CLASS(HeaderBaseTest) {
public:
	TEST_METHOD(HeaderBaseSuccess) {
		// Arrange
		std::vector<std::pair<std::string, std::string>> expectedHeaders{
			{ "Host", "localhost:6006" },
			{ "User-Agent", "Mozilla/5.0 (Windows NT 10.0; Win64; x64; rv:59.0) Gecko/20100101 Firefox/59.0" },
			{ "Accept", "text/html,application/xhtml+xml,application/xml;q=0.9,*/*;q=0.8" },
			{ "Accept-Language", "en-US,en;q=0.5" },
			{ "Accept-Encoding", "gzip, deflate" },
			{ "DNT", "1" },
			{ "Connection", "keep-alive" },
			{ "Upgrade-Insecure-Requests", "1" },
		};
		std::string s = " HTTP/1.1\r\n";
		for (auto const& header : expectedHeaders) {
			s += header.first + ": " + header.second + "\r\n";
		}
		s += "\r\n";
		char const* p = s.data();
		HeaderBase headerBase;

		// Act
		bool result = headerBase.CollectHeaders(p);

		// Assert
		Assert::IsTrue(result);
		Assert::AreEqual(p, s.data() + s.size());
	}

	TEST_METHOD(HeaderBaseFail1) {
		// Arrange
		std::vector<std::pair<std::string, std::string>> expectedHeaders{
			{ "Host", "localhost:6006" },
			{ "Bad\nHeader", "1" },
		};
		std::string s;
		for (auto const& header : expectedHeaders) {
			s += header.first + ": " + header.second + "\r\n";
		}
		s += "\r\n";
		char const* p = &s[0];
		HeaderBase headerBase;

		// Act
		bool result = headerBase.CollectHeaders(p);

		// Assert
		Assert::IsFalse(result);
	}

	TEST_METHOD(HeaderBaseFail2) {
		// Arrange
		std::vector<std::pair<std::string, std::string>> expectedHeaders{
			{ "Host", "localhost:6006" },
			{ "Bad\rHeader", "1" },
		};
		std::string s;
		for (auto const& header : expectedHeaders) {
			s += header.first + ": " + header.second + "\r\n";
		}
		s += "\r\n";
		char const* p = &s[0];
		HeaderBase headerBase;

		// Act
		bool result = headerBase.CollectHeaders(p);

		// Assert
		Assert::IsFalse(result);
	}

	TEST_METHOD(HeaderBaseFail3) {
		// Arrange
		std::vector<std::pair<std::string, std::string>> expectedHeaders{
			{ "Host", "localhost:6006" },
			{ "Bad Header", "1" },
		};
		std::string s;
		for (auto const& header : expectedHeaders) {
			s += header.first + ": " + header.second + "\r\n";
		}
		s += "\r\n";
		char const* p = &s[0];
		HeaderBase headerBase;

		// Act
		bool result = headerBase.CollectHeaders(p);

		// Assert
		Assert::IsFalse(result);
	}
	};
}
