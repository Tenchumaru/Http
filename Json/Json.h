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
	void Unget() noexcept;
	Value ParseArray();
	double ParseNumber(char_t ch) noexcept;
	Value ParseObject();
	void AddUnicode(std::string& s);
	std::string ParseString();
	char32_t ParseUnicodeEscape();
	Value ParseValue();
	void SkipWhitespace() noexcept;
};

void WriteJson(std::ostream& os, nullptr_t);
void WriteJson(std::ostream& os, bool b);
void WriteJson(std::ostream& os, std::string const& s);
void WriteJson(std::ostream& os, std::wstring const& s);
void WriteJson(std::ostream& os, char const* s);
void WriteJson(std::ostream& os, wchar_t const* s);

template<typename T>
void WriteJson(std::ostream& os, T const& t) {
	os << t;
}

template<typename T>
void WriteJson(std::ostream& os, std::vector<T> const& m);

template<typename K, typename V>
void WriteJson(std::ostream& os, std::unordered_map<K, V> const& m) {
	os << '{';
	for(typename std::unordered_map<K, V>::const_iterator i= m.cbegin(), x= m.cend(); i != x; ) {
		WriteJson(os, i->first);
		os << ':';
		WriteJson(os, i->second);
		if(++i != x) {
			os << ',';
		}
	}
	os << '}';
}

template<typename T>
void WriteJson(std::ostream& os, std::vector<T> const& m) {
	os << '[';
	for(typename std::vector<T>::const_iterator i= m.cbegin(), x= m.cend(); i != x; ) {
		WriteJson(os, *i);
		if(++i != x) {
			os << ',';
		}
	}
	os << ']';
}
