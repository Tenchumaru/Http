#include "pch.h"
#include "CppUnitTest.h"

using namespace std::literals;
using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace Json_Test {
	TEST_CLASS(WriteTest) {
		char* stringValue = strdup("string");
		wchar_t* wideStringValue = wcsdup(L"string");

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
		std::string s = stringValue;
		WriteJson(ss, s);
		Assert::AreEqual(ss.str(), "\""s + stringValue + '"');
	}

	TEST_METHOD(WriteCharacterPointer) {
		std::stringstream ss;
		char* p = stringValue;
		WriteJson(ss, p);
		Assert::AreEqual(ss.str(), "\""s + stringValue + '"');
	}

	TEST_METHOD(WriteCharacterConstantPointer) {
		std::stringstream ss;
		char const* p = stringValue;
		WriteJson(ss, p);
		Assert::AreEqual(ss.str(), "\""s + stringValue + '"');
	}

	TEST_METHOD(WriteCharacter) {
		std::stringstream ss;
		char ch = 'a';
		WriteJson(ss, ch);
		Assert::AreEqual(ss.str().c_str(), "\"a\"");
	}

	TEST_METHOD(WriteFloat) {
		std::stringstream ss;
		float d = 2.25f;
		WriteJson(ss, d);
		Assert::AreEqual(ss.str().c_str(), "2.25");
	}

	TEST_METHOD(WriteWideString) {
		std::stringstream ss;
		std::wstring s = wideStringValue;
		WriteJson(ss, s);
		Assert::AreEqual(ss.str(), "\""s + stringValue + '"');
	}

	TEST_METHOD(WriteWideCharacterPointer) {
		std::stringstream ss;
		wchar_t* p = wideStringValue;
		WriteJson(ss, p);
		Assert::AreEqual(ss.str(), "\""s + stringValue + '"');
	}

	TEST_METHOD(WriteWideCharacterConstantPointer) {
		std::stringstream ss;
		wchar_t const* p = wideStringValue;
		WriteJson(ss, p);
		Assert::AreEqual(ss.str(), "\""s + stringValue + '"');
	}

	TEST_METHOD(WriteWideCharacter) {
		std::stringstream ss;
		wchar_t ch = L'a';
		WriteJson(ss, ch);
		Assert::AreEqual(ss.str().c_str(), "\"a\"");
	}

	TEST_METHOD(WriteDouble) {
		std::stringstream ss;
		double d = 1.1e307;
		WriteJson(ss, d);
		Assert::AreEqual(ss.str().c_str(), "1.1e+307");
	}

	TEST_METHOD(WriteLongLong) {
		std::stringstream ss;
		std::int64_t value = -17;
		WriteJson(ss, value);
		Assert::AreEqual(ss.str().c_str(), "-17");
	}

	TEST_METHOD(WriteLong) {
		std::stringstream ss;
		long value = -17;
		WriteJson(ss, value);
		Assert::AreEqual(ss.str().c_str(), "-17");
	}

	TEST_METHOD(WriteInteger) {
		std::stringstream ss;
		int value = -17;
		WriteJson(ss, value);
		Assert::AreEqual(ss.str().c_str(), "-17");
	}

	TEST_METHOD(WriteShort) {
		std::stringstream ss;
		short value = -17;
		WriteJson(ss, value);
		Assert::AreEqual(ss.str().c_str(), "-17");
	}

	TEST_METHOD(WriteSignedByte) {
		std::stringstream ss;
		std::int8_t value = -17;
		WriteJson(ss, value);
		Assert::AreEqual(ss.str().c_str(), "-17");
	}

	TEST_METHOD(WriteUnsignedLongLong) {
		std::stringstream ss;
		std::uint64_t value = 17;
		WriteJson(ss, value);
		Assert::AreEqual(ss.str().c_str(), "17");
	}

	TEST_METHOD(WriteUnsignedLong) {
		std::stringstream ss;
		unsigned long value = 17;
		WriteJson(ss, value);
		Assert::AreEqual(ss.str().c_str(), "17");
	}

	TEST_METHOD(WriteUnsignedInteger) {
		std::stringstream ss;
		unsigned value = 17;
		WriteJson(ss, value);
		Assert::AreEqual(ss.str().c_str(), "17");
	}

	TEST_METHOD(WriteUnsignedShort) {
		std::stringstream ss;
		unsigned short value = 17;
		WriteJson(ss, value);
		Assert::AreEqual(ss.str().c_str(), "17");
	}

	TEST_METHOD(WriteByte) {
		std::stringstream ss;
		std::uint8_t value = 17;
		WriteJson(ss, value);
		Assert::AreEqual(ss.str().c_str(), "17");
	}

	TEST_METHOD(WriteFalse) {
		std::stringstream ss;
		bool value = false;
		WriteJson(ss, value);
		Assert::AreEqual(ss.str().c_str(), "false");
	}

	TEST_METHOD(WriteTrue) {
		std::stringstream ss;
		bool value = true;
		WriteJson(ss, value);
		Assert::AreEqual(ss.str().c_str(), "true");
	}

	TEST_METHOD(WriteNullPointer) {
		std::stringstream ss;
		nullptr_t value = nullptr;
		WriteJson(ss, value);
		Assert::AreEqual(ss.str().c_str(), "null");
	}

	TEST_METHOD(WriteObjectPointer) {
		std::stringstream ss;
		nullptr_t value1 = nullptr;
		bool value2 = true;
		float value3 = 2.25;
		char* value4 = stringValue;
		std::array<int, 3> value5{ 3, 6, 9 };
		std::map<int, char const*> value6{ { 27, "twenty-seven" } };
		WriteJsonObject(ss, "1", value1, "2", value2, "3", value3, "4", value4, "5", value5, "6", value6);
		Assert::AreEqual(ss.str(), "{\"1\":null,\"2\":true,\"3\":2.25,\"4\":\""s + stringValue + "\",\"5\":[3,6,9],\"6\":{\"27\":\"twenty-seven\"}}");
	}

	TEST_METHOD(WriteIterableArrayVector) {
		std::stringstream ss;
		std::array<std::vector<double>, 3> mv{
			std::vector<double>{ .1, .2, .3 },
			std::vector<double>{ .1, .2, .3 },
			std::vector<double>{ .1, .2, .3 },
		};
		WriteJson(ss, mv);
		Assert::AreEqual(ss.str().c_str(), "[[0.1,0.2,0.3],[0.1,0.2,0.3],[0.1,0.2,0.3]]");
	}

	TEST_METHOD(WriteArrayVector) {
		std::stringstream ss;
		std::vector<double> mv[]{
			std::vector<double>{ .1, .2, .3 },
			std::vector<double>{ .1, .2, .3 },
			std::vector<double>{ .1, .2, .3 },
		};
		WriteJson(ss, mv);
		Assert::AreEqual(ss.str().c_str(), "[[0.1,0.2,0.3],[0.1,0.2,0.3],[0.1,0.2,0.3]]");
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

	TEST_METHOD(WriteValue) {
		std::stringstream ss;
		Value value = Value(2.25);
		WriteJson(ss, value);
		Assert::AreEqual(ss.str().c_str(), "2.25");
	}
	};
}
