#include "pch.h"
#include "CppUnitTest.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace Json_Test {
	TEST_CLASS(WriteTest) {
public:

	TEST_METHOD(WriteUnorderedMap) {
		std::stringstream ss;
		std::unordered_map<int, std::string> u{
			{ 1, "one" },
			{ 2, "two" },
			{ 3, "three" },
		};
		WriteJson(ss, u);
		Assert::AreEqual(ss.str().c_str(), "{\"1\":\"one\",\"2\":\"two\",\"3\":\"three\"}");
	}

	TEST_METHOD(WriteMap) {
		std::stringstream ss;
		std::map<std::string, char const*> m{
			{ "1", "one" },
			{ "2", "two" },
			{ "3", "three" },
		};
		WriteJson(ss, m);
		Assert::AreEqual(ss.str().c_str(), "{\"1\":\"one\",\"2\":\"two\",\"3\":\"three\"}");
	}

	TEST_METHOD(WriteString) {
		std::stringstream ss;
		std::string s = "string";
		WriteJson(ss, s);
		Assert::AreEqual(ss.str().c_str(), "\"string\"");
	}

	TEST_METHOD(WriteCharacter) {
		std::stringstream ss;
		char ch = 'a';
		WriteJson(ss, ch);
		Assert::AreEqual(ss.str().c_str(), "\"a\"");
	}

	TEST_METHOD(WriteDouble) {
		std::stringstream ss;
		double d = 1.1e307;
		WriteJson(ss, d);
		Assert::AreEqual(ss.str().c_str(), "1.1e+307");
	}

	TEST_METHOD(WriteUnsignedShort) {
		std::stringstream ss;
		unsigned short us = 17;
		WriteJson(ss, us);
		Assert::AreEqual(ss.str().c_str(), "17");
	}

	TEST_METHOD(WriteMapVector) {
		std::stringstream ss;
		std::map<int, std::vector<double>> mv{
			{ 1, { .1, .2, .3 }},
			{ 2, { .1, .2, .3 }},
			{ 3, { .1, .2, .3 }},
		};
		WriteJson(ss, mv);
		Assert::AreEqual(ss.str().c_str(), "{\"1\":[0.1,0.2,0.3],\"2\":[0.1,0.2,0.3],\"3\":[0.1,0.2,0.3]}");
	}

	TEST_METHOD(WriteVectorMap) {
		std::stringstream ss;
		std::vector<std::map<unsigned char, double>> vm{
			{{ 1, .1 }},
			{{ 2, .2 }},
			{{ 3, .3 }},
		};
		WriteJson(ss, vm);
		Assert::AreEqual(ss.str().c_str(), "[{\"1\":0.1},{\"2\":0.2},{\"3\":0.3}]");
	}
	};
}
