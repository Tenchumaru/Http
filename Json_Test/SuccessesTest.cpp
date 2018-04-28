#include "stdafx.h"
#include "CppUnitTest.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace Json_Test {
	TEST_CLASS(SuccessesTest) {
public:

	TEST_METHOD(SuccessesGoogleCalendar) {
		std::ifstream testCases("../../Json_test/google-calendar.json");
		if(!testCases) {
			throw std::runtime_error("cannot find google-calendar.json");
		}
		std::string line;
		if(!std::getline(testCases, line)) {
			throw std::runtime_error("cannot read google-calendar.json");
		}
		auto result= Parser::Parse(line.c_str());
		Assert::AreEqual(result.ToObject()["version"].ToString().c_str(), "1.0");
		Assert::AreEqual(result.ToObject()["encoding"].ToString().c_str(), "UTF-8");
		Assert::AreEqual(result.ToObject()["feed"].ToObject()["link"].ToArray()[0].ToObject()["rel"].ToString().c_str(), "alternate");
	}
	};
}
