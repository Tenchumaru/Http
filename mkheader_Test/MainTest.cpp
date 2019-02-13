#include "stdafx.h"
#include "CppUnitTest.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace mkheader_Test {
#include "Basic.inl"

	TEST_CLASS(MainTest) {
public:

	TEST_METHOD(Basic) {
		Assert::IsTrue(CollectHeaders("Content-Type: test\r\nMy-Custom-Header: test\r\nOther: dont-care\r\n\r\n"));
		std::string contentType(HcontentType.first, HcontentType.second);
		Assert::AreEqual(std::string("test"), contentType);
		std::string myCustomHeader(HmyCustomHeader.first, HmyCustomHeader.second);
		Assert::AreEqual(std::string("test"), myCustomHeader);
	}
	};
}