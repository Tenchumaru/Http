#include "stdafx.h"
#include "CppUnitTest.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace mkquery_Test {
#include "Basic.inl"

	TEST_CLASS(MainTest) {
public:

	TEST_METHOD(Basic) {
		Assert::IsTrue(CollectQuery("?one=1&ones=11&two=2&ones=12&three=3\r"));
		std::string one(Qone.first, Qone.second);
		Assert::AreEqual(std::string("1"), one);
		std::string two(Qtwo.first, Qtwo.second);
		Assert::AreEqual(std::string("2"), two);
		std::vector<std::string> ones;
		std::transform(Qones.cbegin(), Qones.cend(), std::back_inserter(ones), [](xstring const& s) {
			return std::string(s.first, s.second);
		});
		auto expected = std::vector<std::string>{ "11", "12" };
		Assert::IsTrue(std::equal(expected.cbegin(), expected.cend(), ones.cbegin()));
	}
	};
}