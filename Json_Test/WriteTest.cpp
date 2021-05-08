#include "pch.h"
#include "CppUnitTest.h"

using namespace std::literals;
using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace Json_Test {
	TEST_CLASS(WriteTest) {
		char* stringValue = strdup("string");
		wchar_t* wideStringValue = wcsdup(L"string");

		template<typename T, typename U>
		void WriteStringImpl(U u) {
			std::stringstream ss;
			T s = u;
			WriteJson(ss, s);
			Assert::AreEqual("\""s + stringValue + '"', ss.str());
		}

public:
	TEST_METHOD(WriteUnorderedMap) {
		std::stringstream ss;
		std::unordered_map<int, std::string> u{
			{ 1, "one" },
			{ 2, "two" },
			{ 3, "three" },
		};
		WriteJson(ss, u);
		Assert::AreEqual("{\"1\":\"one\",\"2\":\"two\",\"3\":\"three\"}"s, ss.str());
	}

	TEST_METHOD(WriteMap) {
		std::stringstream ss;
		std::map<std::string, char const*> m{
			{ "1", "one" },
			{ "2", "two" },
			{ "3", "three" },
		};
		WriteJson(ss, m);
		Assert::AreEqual("{\"1\":\"one\",\"2\":\"two\",\"3\":\"three\"}"s, ss.str());
	}

	TEST_METHOD(WriteString) {
		WriteStringImpl<std::string>(stringValue);
	}

	TEST_METHOD(WriteCharacterPointer) {
		WriteStringImpl<char*>(stringValue);
	}

	TEST_METHOD(WriteCharacterConstantPointer) {
		WriteStringImpl<char const*>(stringValue);
	}

	TEST_METHOD(WriteCharacter) {
		std::stringstream ss;
		char ch = 'a';
		WriteJson(ss, ch);
		Assert::AreEqual("\"a\""s, ss.str());
	}

	TEST_METHOD(WriteFloat) {
		std::stringstream ss;
		float d = 2.25f;
		WriteJson(ss, d);
		Assert::AreEqual("2.25"s, ss.str());
	}

	TEST_METHOD(WriteWideString) {
		WriteStringImpl<std::wstring>(wideStringValue);
	}

	TEST_METHOD(WriteWideCharacterPointer) {
		WriteStringImpl<wchar_t*>(wideStringValue);
	}

	TEST_METHOD(WriteWideCharacterConstantPointer) {
		WriteStringImpl<wchar_t const*>(wideStringValue);
	}

	TEST_METHOD(WriteWideCharacter) {
		std::stringstream ss;
		wchar_t ch = L'a';
		WriteJson(ss, ch);
		Assert::AreEqual("\"a\""s, ss.str());
	}

	TEST_METHOD(WriteDouble) {
		std::stringstream ss;
		double d = 1.1e307;
		WriteJson(ss, d);
		Assert::AreEqual("1.1e+307"s, ss.str());
	}

	TEST_METHOD(WriteLongLong) {
		std::stringstream ss;
		std::int64_t value = -17;
		WriteJson(ss, value);
		Assert::AreEqual("-17"s, ss.str());
	}

	TEST_METHOD(WriteLong) {
		std::stringstream ss;
		long value = -17;
		WriteJson(ss, value);
		Assert::AreEqual("-17"s, ss.str());
	}

	TEST_METHOD(WriteInteger) {
		std::stringstream ss;
		int value = -17;
		WriteJson(ss, value);
		Assert::AreEqual("-17"s, ss.str());
	}

	TEST_METHOD(WriteShort) {
		std::stringstream ss;
		short value = -17;
		WriteJson(ss, value);
		Assert::AreEqual("-17"s, ss.str());
	}

	TEST_METHOD(WriteSignedByte) {
		std::stringstream ss;
		std::int8_t value = -17;
		WriteJson(ss, value);
		Assert::AreEqual("-17"s, ss.str());
	}

	TEST_METHOD(WriteUnsignedLongLong) {
		std::stringstream ss;
		std::uint64_t value = 17;
		WriteJson(ss, value);
		Assert::AreEqual("17"s, ss.str());
	}

	TEST_METHOD(WriteUnsignedLong) {
		std::stringstream ss;
		unsigned long value = 17;
		WriteJson(ss, value);
		Assert::AreEqual("17"s, ss.str());
	}

	TEST_METHOD(WriteUnsignedInteger) {
		std::stringstream ss;
		unsigned value = 17;
		WriteJson(ss, value);
		Assert::AreEqual("17"s, ss.str());
	}

	TEST_METHOD(WriteUnsignedShort) {
		std::stringstream ss;
		unsigned short value = 17;
		WriteJson(ss, value);
		Assert::AreEqual("17"s, ss.str());
	}

	TEST_METHOD(WriteByte) {
		std::stringstream ss;
		std::uint8_t value = 17;
		WriteJson(ss, value);
		Assert::AreEqual("17"s, ss.str());
	}

	TEST_METHOD(WriteFalse) {
		std::stringstream ss;
		bool value = false;
		WriteJson(ss, value);
		Assert::AreEqual("false"s, ss.str());
	}

	TEST_METHOD(WriteTrue) {
		std::stringstream ss;
		bool value = true;
		WriteJson(ss, value);
		Assert::AreEqual("true"s, ss.str());
	}

	TEST_METHOD(WriteNullPointer) {
		std::stringstream ss;
		nullptr_t value = nullptr;
		WriteJson(ss, value);
		Assert::AreEqual("null"s, ss.str());
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
		Assert::AreEqual("{\"1\":null,\"2\":true,\"3\":2.25,\"4\":\""s + stringValue + "\",\"5\":[3,6,9],\"6\":{\"27\":\"twenty-seven\"}}", ss.str());
	}

	TEST_METHOD(WriteIterableArrayVector) {
		std::stringstream ss;
		std::array<std::vector<double>, 3> mv{
			std::vector<double>{ .1, .2, .3 },
			std::vector<double>{ .1, .2, .3 },
			std::vector<double>{ .1, .2, .3 },
		};
		WriteJson(ss, mv);
		Assert::AreEqual("[[0.1,0.2,0.3],[0.1,0.2,0.3],[0.1,0.2,0.3]]"s, ss.str());
	}

	TEST_METHOD(WriteArrayVector) {
		std::stringstream ss;
		std::vector<double> mv[]{
			std::vector<double>{ .1, .2, .3 },
			std::vector<double>{ .1, .2, .3 },
			std::vector<double>{ .1, .2, .3 },
		};
		WriteJson(ss, mv);
		Assert::AreEqual("[[0.1,0.2,0.3],[0.1,0.2,0.3],[0.1,0.2,0.3]]"s, ss.str());
	}

	TEST_METHOD(WriteMapVector) {
		std::stringstream ss;
		std::map<int, std::vector<double>> mv{
			{ 1, { .1, .2, .3 }},
			{ 2, { .1, .2, .3 }},
			{ 3, { .1, .2, .3 }},
		};
		WriteJson(ss, mv);
		Assert::AreEqual("{\"1\":[0.1,0.2,0.3],\"2\":[0.1,0.2,0.3],\"3\":[0.1,0.2,0.3]}"s, ss.str());
	}

	TEST_METHOD(WriteVectorMap) {
		std::stringstream ss;
		std::vector<std::map<unsigned char, double>> vm{
			{{ 1, .1 }},
			{{ 2, .2 }},
			{{ 3, .3 }},
		};
		WriteJson(ss, vm);
		Assert::AreEqual("[{\"1\":0.1},{\"2\":0.2},{\"3\":0.3}]"s, ss.str());
	}

	TEST_METHOD(WriteValue) {
		std::stringstream ss;
		Value value = Value(2.25);
		WriteJson(ss, value);
		Assert::AreEqual("2.25"s, ss.str());
	}
	};
}
