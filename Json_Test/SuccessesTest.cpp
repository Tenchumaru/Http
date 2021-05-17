#include "pch.h"
#include <CppUnitTest.h>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace Json_Test {
	TEST_CLASS(SuccessesTest) {
public:

	TEST_METHOD(Successes) {
		std::ifstream testCases("../../Json_test/successes.json.txt");
		if (!testCases) {
			throw std::runtime_error("cannot find successes.json.txt");
		}
		std::string line, text;
		while (std::getline(testCases, line)) {
			if (line == "-------") {
				Parser::Parse(text.c_str());
				text.clear();
			} else {
				text += '\n';
				text += line;
			}
		}
		Parser::Parse(text.c_str());
	}

	TEST_METHOD(SuccessesArrays) {
		std::ifstream testCases("../../Json_test/arrays.json.txt");
		if (!testCases) {
			throw std::runtime_error("cannot find arrays.json.txt");
		}
		std::string line;
		while (std::getline(testCases, line)) {
			Parser::Parse(line.c_str());
		}
	}

	TEST_METHOD(SuccessesBasics) {
		static char const* basics[] = { "123456789", "-123456789", "1.2345678", "\"abcdef\"", "null", "true", "false" };
		for (auto const* basic : basics) {
			Parser::Parse(basic);
		}
	}

	TEST_METHOD(SuccessesNumbers) {
		static char const* numbers[] = { "2147483647", "-2147483648", "4294967295", "0", "1", "8589934592", "-4294967295",
			"-4294967295", "12345678e-7", "0.12345678e7", "-12345678e-7", "-0.12345678e7" };
		for (auto const* number : numbers) {
			Parser::Parse(number);
		}
	}

	TEST_METHOD(SuccessesObjects) {
		std::ifstream testCases("../../Json_test/objects.json.txt");
		if (!testCases) {
			throw std::runtime_error("cannot find objects.json.txt");
		}
		std::string line, text;
		while (std::getline(testCases, line)) {
			if (line == "-------") {
				Parser::Parse(text.c_str());
				text.clear();
			} else {
				text += '\n';
				text += line;
			}
		}
		Parser::Parse(text.c_str());
	}

	TEST_METHOD(SuccessesStrings) {
		std::ifstream testCases("../../Json_test/strings.json.txt");
		if (!testCases) {
			throw std::runtime_error("cannot find strings.json.txt");
		}
		std::string line;
		while (std::getline(testCases, line)) {
			Parser::Parse(line.c_str());
		}
	}

	TEST_METHOD(SuccessesLarge) {
		std::ifstream testCases("../../Json_test/large.json");
		if (!testCases) {
			throw std::runtime_error("cannot find large.json");
		}
		std::string line, text;
		while (std::getline(testCases, line)) {
			text += line;
			text += '\n';
		}
		auto result = Parser::Parse(text.c_str());
	}

	TEST_METHOD(SuccessesGoogleCalendar) {
		std::ifstream testCases("../../Json_test/google-calendar.json");
		if (!testCases) {
			throw std::runtime_error("cannot find google-calendar.json");
		}
		std::string line;
		if (!std::getline(testCases, line)) {
			throw std::runtime_error("cannot read google-calendar.json");
		}
		auto result = Parser::Parse(line.c_str());
		Assert::AreEqual(result.ToObject()["version"].ToString().c_str(), "1.0");
		Assert::AreEqual(result.ToObject()["encoding"].ToString().c_str(), "UTF-8");
		Assert::AreEqual(result.ToObject()["feed"].ToObject()["link"].ToArray()[0].ToObject()["rel"].ToString().c_str(), "alternate");
	}
	};
}
