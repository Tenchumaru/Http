#pragma once

class Value {
public:
	enum class Type { Null, Boolean, Number, Array, Object, String };

	explicit Value(nullptr_t) noexcept : type(Type::Null) {}
	explicit Value(bool b) noexcept : number(b), type(Type::Boolean) {}
	explicit Value(double number) noexcept : number(number), type(Type::Number) {}
	explicit Value(std::string&& string) : string(new std::string(string)), type(Type::String) {}
	Value()= default;
	Value(Value const&)= default;
	Value(Value&&)= default;
	~Value()= default;
	Value& operator=(Value const&)= default;
	Value& operator=(Value&&)= default;

	bool ToBoolean() const noexcept { return static_cast<bool>(number); }
	double ToNumber() const noexcept { return number; }
	std::string const& ToString() const { return *string; }
	std::unordered_map<std::string, Value>& ToObject() const { return *object; }
	std::vector<Value>& ToArray() const { return *array; }
	Type GetType() const noexcept { return type; }
	void Write(std::ostream& os) const;

protected:
	friend class Parser;

	union {
		double number;
		std::string* string;
		std::unordered_map<std::string, Value>* object;
		std::vector<Value>* array;
	};
	Type type;

	static void Clear(Value& value) noexcept;
};

class RootValue : Value {
public:
	RootValue(Value const& value) noexcept;
	RootValue(RootValue const&)= delete;
	RootValue(RootValue&& that) noexcept;
	RootValue& operator=(RootValue const&)= delete;
	RootValue& operator=(RootValue&& that) noexcept;
	~RootValue();
	using Value::ToBoolean;
	using Value::ToNumber;
	using Value::ToString;
	using Value::ToObject;
	using Value::ToArray;
	using Value::GetType;
	using Value::Write;
};

class Parser {
public:
	Parser(Parser const&)= default;
	Parser(Parser&&)= default;
	~Parser()= default;
	Parser& operator=(Parser const&)= default;
	Parser& operator=(Parser&&)= default;
	static RootValue Parse(char const* s);

private:
	using char_t= std::string::value_type;

	char const* text;
	char const* next;

	Parser() {}
	RootValue InternalParse(char const* s);
	char_t Get(bool acceptWhitespace= false) noexcept;
	char_t Next(bool acceptWhitespace= false) noexcept;
	Value ParseArray();
	double ParseNumber(char_t ch);
	Value ParseObject();
	void AddUnicode(std::string& s);
	std::string ParseString();
	char32_t ParseUnicodeEscape();
	Value ParseValue();
	void SkipWhitespace() noexcept;
};

void WriteJson(std::ostream& os, double d);
void WriteJson(std::ostream& os, char const* s);
void WriteJson(std::ostream& os, wchar_t const* s);

inline void WriteJson(std::ostream& os, nullptr_t) {
	os << "null";
}

inline void WriteJson(std::ostream& os, bool b) {
	auto const* s = b ? "true" : "false";
	os << s;
}

inline void WriteJson(std::ostream& os, std::string const& s) {
	WriteJson(os, s.c_str());
}

inline void WriteJson(std::ostream& os, std::wstring const& s) {
	WriteJson(os, s.c_str());
}

inline void WriteJson(std::ostream& os, char ch) {
	char s[2]{ ch };
	WriteJson(os, s);
}

inline void WriteJson(std::ostream& os, wchar_t ch) {
	wchar_t s[2]{ ch };
	WriteJson(os, s);
}

template <typename...>
using void_t = void;

template <typename, typename = void>
struct has_mapped_type : std::false_type {};

template <typename T>
struct has_mapped_type<T, void_t<typename T::mapped_type>> : std::true_type {};

template <typename, typename = void>
struct has_const_iterator : std::false_type {};

template <typename T>
struct has_const_iterator<T, void_t<typename T::const_iterator>> : std::true_type {};

template<typename T>
using is_character = std::integral_constant<bool, std::is_same_v<char, T> || std::is_same_v<char16_t, T> ||
	std::is_same_v<char32_t, T> || std::is_same_v<wchar_t, T>>;

template<typename T>
using is_string = std::integral_constant<bool, std::is_same_v<std::string, T> || std::is_same_v<std::wstring, T>>;

template<typename T>
using is_integer = std::integral_constant<bool, std::is_integral_v<T> && !std::is_same_v<bool, T> && !is_character<T>::value>;

template<typename T>
using is_iterable = std::integral_constant<bool, has_const_iterator<T>::value && !has_mapped_type<T>::value && !is_string<T>::value>;

template<typename T>
std::enable_if_t<is_integer<T>::value> WriteJson(std::ostream& os, T const& t) {
	os << t;
}

template<typename T>
std::enable_if_t<has_mapped_type<T>::value && is_string<typename T::key_type>::value> WriteJson(std::ostream& os, T const& m) {
	os << '{';
	for(typename T::const_iterator it= m.cbegin(), end= m.cend(); it != end; ) {
		WriteJson(os, it->first);
		os << ':';
		WriteJson(os, it->second);
		if(++it != end) {
			os << ',';
		}
	}
	os << '}';
}

template<typename T>
std::enable_if_t<has_mapped_type<T>::value && is_integer<typename T::key_type>::value> WriteJson(std::ostream& os, T const& m) {
	os << '{';
	for(typename T::const_iterator it= m.cbegin(), end= m.cend(); it != end; ) {
		WriteJson(os, std::to_string(it->first));
		os << ':';
		WriteJson(os, it->second);
		if(++it != end) {
			os << ',';
		}
	}
	os << '}';
}

template<typename T>
std::enable_if_t<is_iterable<T>::value> WriteJson(std::ostream& os, T const& m) {
	os << '[';
	for(typename T::const_iterator it= m.cbegin(), end= m.cend(); it != end; ) {
		WriteJson(os, *it);
		if(++it != end) {
			os << ',';
		}
	}
	os << ']';
}
