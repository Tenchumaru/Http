#include "stdafx.h"
#include "CppUnitTest.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace mkheader_Test {
#include "Basic.inl"

	TEST_CLASS(MainTest) {
public:

	TEST_METHOD(Basic) {
		Assert::IsTrue(CollectHeaders("Content-Type: test\r\nMy-Custom-Header: test\r\nOther: dont-care\r\n\r\n"));
		std::string contentType(HContent_Type.first, HContent_Type.second);
		Assert::AreEqual(std::string("test"), contentType);
		std::string myCustomHeader(Hmy_custom_header.first, Hmy_custom_header.second);
		Assert::AreEqual(std::string("test"), myCustomHeader);
	}
	};
}