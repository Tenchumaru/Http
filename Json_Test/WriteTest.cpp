#include "pch.h"
#include <CppUnitTest.h>

using namespace std::literals;
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

	template<typename T>
	void WriteSomething(T value, std::string const& expectedValue) {
		std::stringstream ss;
		WriteJson(ss, value);
		auto actualValue = ss.str();
		Assert::AreEqual(expectedValue, actualValue);
	}
	TEST_METHOD(WriteWideCharacter) {
		WriteSomething(L'a', "\"a\"");
	}

	TEST_METHOD(WriteDouble) {
		WriteSomething(1.1e307, "1.1e+307");
	}

	TEST_METHOD(WriteLongLong) {
		WriteSomething(std::int64_t(-17), "-17");
	}

	TEST_METHOD(WriteLong) {
		WriteSomething(-17l, "-17");
	}

	TEST_METHOD(WriteInteger) {
		WriteSomething(-17, "-17");
	}

	TEST_METHOD(WriteShort) {
		WriteSomething(short(-17), "-17");
	}

	TEST_METHOD(WriteSignedByte) {
		WriteSomething(std::int8_t(-17), "-17");
	}

	TEST_METHOD(WriteUnsignedLongLong) {
		WriteSomething(std::uint64_t(17), "17");
	}

	TEST_METHOD(WriteUnsignedLong) {
		WriteSomething(17ul, "17");
	}

	TEST_METHOD(WriteUnsignedInteger) {
		WriteSomething(17u, "17");
	}

	TEST_METHOD(WriteUnsignedShort) {
		WriteSomething(unsigned short(17), "17");
	}

	TEST_METHOD(WriteByte) {
		WriteSomething(std::uint8_t(17), "17");
	}

	TEST_METHOD(WriteFalse) {
		WriteSomething(false, "false");
	}

	TEST_METHOD(WriteTrue) {
		WriteSomething(true, "true");
	}

	TEST_METHOD(WriteNullPointer) {
		WriteSomething(nullptr, "null");
	}

	TEST_METHOD(WriteObject) {
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
		std::array<std::vector<double>, 3> mv{
			std::vector<double>{ .1, .2, .3 },
			std::vector<double>{ .1, .2, .3 },
			std::vector<double>{ .1, .2, .3 },
		};
		WriteSomething(mv, "[[0.1,0.2,0.3],[0.1,0.2,0.3],[0.1,0.2,0.3]]");
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
		std::map<int, std::vector<double>> mv{
			{ 1, { .1, .2, .3 }},
			{ 2, { .1, .2, .3 }},
			{ 3, { .1, .2, .3 }},
		};
		WriteSomething(mv, "{\"1\":[0.1,0.2,0.3],\"2\":[0.1,0.2,0.3],\"3\":[0.1,0.2,0.3]}");
	}

	TEST_METHOD(WriteVectorMap) {
		std::vector<std::map<unsigned char, double>> vm{
			{{ 1, .1 }},
			{{ 2, .2 }},
			{{ 3, .3 }},
		};
		WriteSomething(vm, "[{\"1\":0.1},{\"2\":0.2},{\"3\":0.3}]");
	}

	TEST_METHOD(WriteNullValue) {
		WriteSomething(Value(nullptr), "null");
	}

	TEST_METHOD(WriteBooleanValue) {
		WriteSomething(Value(true), "true");
	}

	TEST_METHOD(WriteNumberValue) {
		WriteSomething(Value(2.25), "2.25");
	}

	TEST_METHOD(WriteArrayValue) {
		std::vector<Value> v;
		v.emplace_back(Value(nullptr));
		v.emplace_back(Value(true));
		v.emplace_back(Value(2.25));
		WriteSomething(Value(std::move(v)), "[null,true,2.25]");
	}

	TEST_METHOD(WriteObjectValue) {
		std::unordered_map<std::string, Value> m;
		m.emplace("1", Value(nullptr));
		m.emplace("2", Value(true));
		m.emplace("3", Value(2.25));
		WriteSomething(Value(std::move(m)), "{\"1\":null,\"2\":true,\"3\":2.25}");
	}

	TEST_METHOD(WriteCharacterConstantPointerValue) {
		WriteSomething(Value("string"), "\""s + stringValue + '"');
	}

	TEST_METHOD(WriteStringValue) {
		WriteSomething(Value("string"s), "\""s + stringValue + '"');
	}

private:
	char* stringValue = strdup("string");
	wchar_t* wideStringValue = wcsdup(L"string");

	template<typename T, typename U>
	void WriteStringImpl(U u) {
		std::stringstream ss;
		T s = u;
		WriteJson(ss, s);
		Assert::AreEqual("\""s + stringValue + '"', ss.str());
	}
	};
}
