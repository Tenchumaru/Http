#include "pch.h"
#include <CppUnitTest.h>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace Json_Test {
	TEST_CLASS(FailuresTest) {
public:
	TEST_METHOD(Failures) {
		std::ifstream testCases("../../Json_test/failures.json.txt");
		if (!testCases) {
			throw std::runtime_error("cannot find failures.json.txt");
		}
		std::string line;
		auto fn = [&line] {
			Parser::Parse(line.c_str());
		};
		while (std::getline(testCases, line)) {
			Assert::ExpectException<std::runtime_error, decltype(fn)>(fn);
		}
	}

	TEST_METHOD(FailuresLineBreaks) {
		char const* text1 = "[\"line\nbreak\"]";
		auto fn1 = [text1] { Parser::Parse(text1); };
		Assert::ExpectException<std::runtime_error, decltype(fn1)>(fn1);

		char const* text2 = "[\"line\\\nbreak\"]";
		auto fn2 = [text2] { Parser::Parse(text2); };
		Assert::ExpectException<std::runtime_error, decltype(fn2)>(fn2);
	}

	TEST_METHOD(JsonFailuresInfinity) {
		auto fn = [] {
			double n = 1.79769313486231e+308;
			n += 1e+307;
			WriteJson(std::cout, n);
		};
		Assert::ExpectException<std::runtime_error, decltype(fn)>(fn);
	}
	};
}
