#include "pch.h"
#include <CppUnitTest.h>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace Json_Test {
	TEST_CLASS(RoundtripsTest) {
public:

	TEST_METHOD(Roundtrips) {
		std::ifstream testCases("../../Json_test/roundtrips.json.txt");
		if (!testCases) {
			throw std::runtime_error("cannot find roundtrips.json.txt");
		}
		std::string line;
		while (std::getline(testCases, line)) {
			auto json = Parser::Parse(line.c_str());
			std::stringstream ss;
			json.Write(ss);
			Assert::AreEqual(line, ss.str());
		}
	}
	};
}
