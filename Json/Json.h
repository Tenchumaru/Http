#pragma once

// TODO:  consider using the implementation in Yarborough/Adrezdi.

class Value {
public:
	enum class Type { Null, Boolean, Number, Array, Object, String };

	Value() noexcept;
	Value(Value&& that) noexcept;
	Value(Value const&) = delete;
	explicit Value(nullptr_t) noexcept : type(Type::Null) {}
	explicit Value(bool b) noexcept : number(b), type(Type::Boolean) {}
	explicit Value(double number) noexcept : number(number), type(Type::Number) {}
	explicit Value(std::string&& string) : string(new std::string(string)), type(Type::String) {}
	explicit Value(char const* string) : Value(std::string(string)) {}
	explicit Value(std::unordered_map<std::string, Value>&& values);
	explicit Value(std::vector<Value>&& values);
	Value& operator=(Value&& that) noexcept;
	Value& operator=(Value const&) = delete;
	~Value();

	bool ToBoolean() const noexcept { return static_cast<bool>(number); }
	double ToNumber() const noexcept { return number; }
	std::string const& ToString() const noexcept { return *string; }
	std::unordered_map<std::string, Value>& ToObject() const noexcept { return *object; }
	std::vector<Value>& ToArray() const noexcept { return *array; }
	Type GetType() const noexcept { return type; }
	void Write(std::ostream& os) const;

private:
	friend class Parser;

	union {
		double number;
		std::string* string;
		std::unordered_map<std::string, Value>* object;
		std::vector<Value>* array;
	};
	Type type;
};

class Parser {
public:
	static Value Parse(char const* text);

private:
	using char_t = std::string::value_type;

	char const* text;
	char const* next;

	Parser() = delete;
	Parser(char const* text) noexcept;
	Parser(Parser const&) = delete;
	Parser(Parser&&) = delete;
	~Parser() = default;
	Parser& operator=(Parser const&) = delete;
	Parser& operator=(Parser&&) = delete;
	Value InternalParse();
	char_t Get(bool acceptWhitespace = false) noexcept;
	char_t Next(bool acceptWhitespace = false) noexcept;
	Value ParseArray();
	double ParseNumber(char_t ch);
	Value ParseObject();
	void AddUnicode(std::string& s);
	std::string ParseString();
	char32_t ParseUnicodeEscape();
	Value ParseValue();
	void SkipWhitespace() noexcept;
};

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

void FromJson(Value const& json, bool& b);
void FromJson(Value const& json, double& d);
void FromJson(Value const& json, std::string& s);
void FromJson(Value const& json, void*& p);

template<typename T>
void FromJson(Value const& json, std::vector<T>& out) {
	if (json.GetType() == Value::Type::Array) {
		auto const& in = json.ToArray();
		std::transform(in.cbegin(), in.cend(), std::back_inserter(out), [](Value const& v) {
			T t;
			FromJson(v, t);
			return t;
		});
	} else {
		throw std::runtime_error("json not array");
	}
}

template<typename T>
void FromJson(Value const& json, T& out) {
	if (json.GetType() == Value::Type::Object) {
		auto const& in = json.ToObject();
		for (auto const& v : in) {
			typename T::mapped_type t;
			FromJson(v.second, t);
			out.insert({ v.first, t });
		}
	} else {
		throw std::runtime_error("json not object");
	}
}

void WriteJson(std::ostream& os, double d);
void WriteJson(std::ostream& os, char const* s);
void WriteJson(std::ostream& os, wchar_t const* s);

inline void WriteJson(std::ostream& os, Value const& value) {
	value.Write(os);
}

inline void WriteJson(std::ostream& os, nullptr_t) {
	os << "null";
}

template<typename T>
using remove_cv_ref_t = typename std::remove_cv<typename std::remove_reference<T>::type>::type;

template<typename T>
std::enable_if_t<std::is_same_v<std::decay_t<remove_cv_ref_t<T>>, bool>>
WriteJson(std::ostream& os, T b) {
	auto const* const s = b ? "true" : "false";
	os << s;
}

inline void WriteJson(std::ostream& os, std::string const& s) {
	WriteJson(os, s.c_str());
}

inline void WriteJson(std::ostream& os, std::wstring const& s) {
	WriteJson(os, s.c_str());
}

inline void WriteJson(std::ostream& os, char ch) {
	char const s[2]{ ch };
	WriteJson(os, s);
}

inline void WriteJson(std::ostream& os, wchar_t ch) {
	wchar_t const s[2]{ ch };
	WriteJson(os, s);
}

template<typename T>
std::enable_if_t<is_integer<T>::value> WriteJson(std::ostream& os, T const& t) {
	os << t;
}

template<>
inline void WriteJson(std::ostream& os, std::uint8_t const& t) {
	os << static_cast<unsigned>(t);
}

template<>
inline void WriteJson(std::ostream& os, std::int8_t const& t) {
	os << static_cast<int>(t);
}

template<typename T>
void WriteJsonObjectMember(std::ostream& os, std::string const& name, T const& value);

template<typename T>
std::enable_if_t<has_mapped_type<T>::value&& is_string<typename T::key_type>::value> WriteJson(std::ostream& os, T const& m) {
	os << '{';
	for (typename T::const_iterator it = m.cbegin(), end = m.cend(); it != end; ) {
		WriteJsonObjectMember(os, it->first, it->second);
		if (++it != end) {
			os << ',';
		}
	}
	os << '}';
}

template<typename T>
std::enable_if_t<has_mapped_type<T>::value&& is_integer<typename T::key_type>::value> WriteJson(std::ostream& os, T const& m) {
	os << '{';
	for (typename T::const_iterator it = m.cbegin(), end = m.cend(); it != end; ) {
		WriteJsonObjectMember(os, std::to_string(it->first), it->second);
		if (++it != end) {
			os << ',';
		}
	}
	os << '}';
}

template<typename T>
std::enable_if_t<is_iterable<T>::value> WriteJson(std::ostream& os, T const& m) {
	os << '[';
	for (typename T::const_iterator it = m.cbegin(), end = m.cend(); it != end; ) {
		WriteJson(os, *it);
		if (++it != end) {
			os << ',';
		}
	}
	os << ']';
}

template<typename T, size_t N>
std::enable_if_t<is_iterable<T>::value> WriteJson(std::ostream& os, T const (&m)[N]) {
	os << '[';
	for (size_t i = 0; i < N; ++i) {
		WriteJson(os, m[i]);
		if (i < N - 1) {
			os << ',';
		}
	}
	os << ']';
}

template<typename T, typename... Types>
void WriteJsonObjectMember(std::ostream& os, std::string const& name, T const& value, Types... rest) {
	WriteJsonObjectMember(os, name, value);
	os << ',';
	WriteJsonObjectMember(os, rest...);
}

template<typename T>
void WriteJsonObjectMember(std::ostream& os, std::string const& name, T const& value) {
	WriteJson(os, name);
	os << ':';
	WriteJson(os, value);
}

template<typename... Types>
void WriteJsonObject(std::ostream& os, Types... rest) {
	os << '{';
	WriteJsonObjectMember(os, rest...);
	os << '}';
}
