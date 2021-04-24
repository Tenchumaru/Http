#include "pch.h"
#include "../Http/Uri.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace Http_Test {
	TEST_CLASS(UriTest) {
public:

	TEST_METHOD(UriCreateEmpty) {
		// Arrange
		std::string s;
		Uri uri;

		// Act
		auto result = Uri::Create(s, uri);

		// Assert
		Assert::IsFalse(result);
	}

	TEST_METHOD(UriCreateFull) {
		// Arrange
		std::string s = "scheme://username:password@host:port/the/path?name1=value1&name2=value2#fragment";
		Uri uri;

		// Act
		auto result = Uri::Create(s, uri);

		// Assert
		Assert::IsTrue(result);
		Assert::AreEqual("/the/path", uri.Path.c_str());
		Assert::AreEqual("username:password@host:port", uri.Authority.c_str());
		Assert::AreEqual("fragment", uri.Fragment.c_str());
		Assert::AreEqual("host", uri.Host.c_str());
		Assert::AreEqual("port", uri.Port.c_str());
		Assert::AreEqual(2ull, uri.Query.size());
		auto it = uri.Query.cbegin();
		Assert::AreEqual("name1", it->first.c_str());
		Assert::AreEqual("value1", it->second.c_str());
		++it;
		Assert::AreEqual("name2", it->first.c_str());
		Assert::AreEqual("value2", it->second.c_str());
		Assert::AreEqual("name1=value1&name2=value2", uri.RawQuery.c_str());
		Assert::AreEqual("scheme", uri.Scheme.c_str());
	}

	TEST_METHOD(UriCreatePathOnly) {
		// Arrange
		std::string s = "/this/is/only/a/path";
		Uri uri;

		// Act
		auto result = Uri::Create(s, uri);

		// Assert
		Assert::IsTrue(result);
		Assert::AreEqual(s, uri.Path);
		Assert::IsTrue(uri.Authority.empty());
		Assert::IsTrue(uri.Fragment.empty());
		Assert::IsTrue(uri.Host.empty());
		Assert::IsTrue(uri.Port.empty());
		Assert::IsTrue(uri.Query.empty());
		Assert::IsTrue(uri.RawQuery.empty());
		Assert::IsTrue(uri.Scheme.empty());
	}

	TEST_METHOD(UriDecodeResource) {
		// Arrange
		std::string s = "/this/is%0aonly%0Da+path";

		// Act
		auto result = Uri::DecodeResource(s, false);

		// Assert
		Assert::IsTrue(result);
		Assert::AreEqual("/this/is\nonly\ra+path", s.c_str());
	}

	TEST_METHOD(UriDecodeQueryResource) {
		// Arrange
		std::string s = "/this/is%0aonly%0Da+path";

		// Act
		auto result = Uri::DecodeResource(s, true);

		// Assert
		Assert::IsTrue(result);
		Assert::AreEqual("/this/is\nonly\ra path", s.c_str());
	}

	TEST_METHOD(UriFailDecodeResource1) {
		// Arrange
		std::string s = "/this/is%0aonly%0Da+pa%th";

		// Act
		auto result = Uri::DecodeResource(s, false);

		// Assert
		Assert::IsFalse(result);
		Assert::AreEqual("/this/is%0aonly%0Da+pa%th", s.c_str());
	}

	TEST_METHOD(UriFailDecodeResource2) {
		// Arrange
		std::string s = "/this/is%0aonly%0Da+path%";

		// Act
		auto result = Uri::DecodeResource(s, false);

		// Assert
		Assert::IsFalse(result);
		Assert::AreEqual("/this/is%0aonly%0Da+path%", s.c_str());
	}

	TEST_METHOD(UriParseNameValue) {
		// Arrange
		std::string s = "/this/is%0ao=nly%0Da+path";
		Uri::pair nameValue;

		// Act
		auto result = Uri::ParseNameValue(s.cbegin(), s.cend(), nameValue);

		// Assert
		Assert::IsTrue(result);
		Assert::AreEqual("/this/is\no", nameValue.first.c_str());
		Assert::AreEqual("nly\ra path", nameValue.second.c_str());
	}

	TEST_METHOD(UriParseQuery) {
		// Arrange
		std::string s = "/this/i=s%0aonl&y%0Da+p=ath";
		Uri::map query;

		// Act
		auto result = Uri::ParseQuery(s, query);

		// Assert
		Assert::IsTrue(result);
		Assert::AreEqual(2ull, query.size());
		auto it = query.cbegin();
		Assert::AreEqual("y\ra p", it->first.c_str());
		Assert::AreEqual("ath", it->second.c_str());
		++it;
		Assert::AreEqual("/this/i", it->first.c_str());
		Assert::AreEqual("s\nonl", it->second.c_str());
	}
	};
}
