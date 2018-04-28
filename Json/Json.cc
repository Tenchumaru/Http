#include "stdafx.h"
#include "Json.h"

void Value::Write(std::ostream& os) const {
	switch(type) {
	case Type::Null:
		WriteJson(os, nullptr);
		break;
	case Type::Boolean:
		WriteJson(os, number ? true : false);
		break;
	case Type::Number:
		WriteJson(os, number);
		break;
	case Type::Array:
		os << '[';
		for(auto it= array->cbegin(), end= array->cend(); it != end; ) {
			it->Write(os);
			if(++it != end) {
				os << ',';
			}
		}
		os << ']';
		break;
	case Type::Object:
		os << '{';
		for(auto it= object->cbegin(), end= object->cend(); it != end; ) {
			WriteJson(os, it->first);
			os << ':';
			it->second.Write(os);
			if(++it != end) {
				os << ',';
			}
		}
		os << '}';
		break;
	case Type::String:
		WriteJson(os, *string);
		break;
	}
}

RootValue::RootValue(RootValue&& that) noexcept {
	static_assert(sizeof(number) >= sizeof(array), "invalid size");
	type= that.GetType();
	number= that.ToNumber();
	that.type= Type::Null;
}

RootValue::RootValue(Value const& value) noexcept {
	type= value.GetType();
	number= value.ToNumber();
}

RootValue& RootValue::operator=(RootValue&& that) noexcept {
	Clear(*this);
	type= that.GetType();
	number= that.ToNumber();
	that.type= Type::Null;
	return *this;
}

RootValue::~RootValue() {
	Clear(*this);
}

void Value::Clear(Value& value) noexcept {
	switch(value.type) {
	case Type::Array:
		for(auto& child : *value.array) {
			Clear(child);
		}
		delete value.array;
		break;
	case Type::Object:
		for(auto& member : *value.object) {
			Clear(member.second);
		}
		delete value.object;
		break;
	case Type::String:
		delete value.string;
		break;
	case Type::Boolean:
	case Type::Null:
	case Type::Number:
		break;
	}
}

RootValue Parser::Parse(char const* s) {
	return Parser{}.InternalParse(s);
}

RootValue Parser::InternalParse(char const* s_) {
	next= text= s_;
	auto const value= ParseValue();
	SkipWhitespace();
	if(*next) {
		throw std::runtime_error("extra input");
	}
	return RootValue(value);
}

Parser::char_t Parser::Get() noexcept {
	SkipWhitespace();
	if(*next) {
		return *next++;
	}
	return '\0';
}

void Parser::Unget() noexcept {
	if(*next) {
		--next;
	}
}

Value Parser::ParseArray() {
	Value rv;
	rv.array= new std::vector<Value>;
	auto ch= Get();
	if(ch != ']') {
		Unget();
		for(;;) {
			rv.array->push_back(ParseValue());
			ch= Get();
			if(ch == ',') {
				continue;
			} else if(ch == ']') {
				break;
			} else {
				throw std::runtime_error("invalid array");
			}
		}
	}
	rv.type= Value::Type::Array;
	return rv;
}

double Parser::ParseNumber(char_t ch) noexcept {
	double number;
	bool isNegative;
	if(ch == '-') {
		number= 0;
		isNegative= true;
	} else {
		number= ch - '0';
		isNegative= false;
	}
	while(ch= Get(), isdigit(ch)) {
		number *= 10;
		number += ch - '0';
	}
	if(ch == '.') {
		double multiplier= .1;
		while(ch= Get(), isdigit(ch)) {
			number += (ch - '0') * multiplier;
			multiplier *= .1;
		}
	}
	if(tolower(ch) == 'e') {
		int exponent= 0;
		while(ch= Get(), isdigit(ch)) {
			exponent *= 10;
			exponent += ch - '0';
		}
		number *= pow(10, exponent);
	}
	Unget();
	return isNegative ? -number : number;
}

Value Parser::ParseObject() {
	Value rv;
	rv.object= new std::unordered_map<std::string, Value>;
	auto ch= Get();
	if(ch != '}') {
		Unget();
		for(;;) {
			if(Get() != '"') {
				throw std::runtime_error("invalid object name");
			}
			auto const name= ParseString();
			if(Get() != ':') {
				throw std::runtime_error("invalid object separator");
			}
			(*rv.object)[name]= ParseValue();
			ch= Get();
			if(ch == ',') {
				continue;
			} else if(ch == '}') {
				break;
			} else {
				throw std::runtime_error("invalid object");
			}
		}
	}
	rv.type= Value::Type::Object;
	return rv;
}

char32_t Parser::ParseUnicodeEscape() {
	char32_t ch= 0;
	for(int i= 0; i < 4; ++i) {
		char d= Get();
		if(!isxdigit(d)) {
			throw std::runtime_error("invalid unicode escape");
		}
		ch <<= 4;
		ch += d >= 'a' ? d - 'a' + 10 : d >= 'A' ? d - 'A' + 10 : d - '0';
	}
	return ch;
}

void Parser::AddUnicode(std::string& s) {
	// Parse the 16-bit unicode escape sequence.  If necessary, parse its
	// surrogate pair.
	char32_t ch= ParseUnicodeEscape();
	if(ch >= 0xd800 && ch <= 0xdbff) {
		if(Get() == '\\' && Get() == 'u') {
			ch= 0x10000 + ((ch & 0x3ff) << 10) + (ParseUnicodeEscape() & 0x3ff);
		} else {
			throw std::runtime_error("unmatched surrogate pair");
		}
	}

	// Convert from UTF-32 to UTF-8.
	if(ch <= 0x7f) {
		s += static_cast<char>(ch);
	} else if(ch <= 0x7ff) {
		s += static_cast<char>(0xc0 | (0x1f & (ch >> 6)));
		s += static_cast<char>(0x80 | (0x3f & ch));
	} else if(ch <= 0xffff) {
		s += static_cast<char>(0xe0 | (0x0f & (ch >> 12)));
		s += static_cast<char>(0x80 | (0x3f & (ch >> 6)));
		s += static_cast<char>(0x80 | (0x3f & ch));
	} else if(ch <= 0x10ffff) {
		s += static_cast<char>(0xf0 | (0x07 & (ch >> 18)));
		s += static_cast<char>(0x80 | (0x3f & (ch >> 12)));
		s += static_cast<char>(0x80 | (0x3f & (ch >> 6)));
		s += static_cast<char>(0x80 | (0x3f & ch));
	}
}

std::string Parser::ParseString() {
	std::string rv;
	for(char ch; ch= Get(), ch != '"'; ) {
		if(ch == '\\') {
			switch(Get()) {
			case '"':
				rv += '"';
				break;
			case '/':
				rv += '/';
				break;
			case '\\':
				rv += '\\';
				break;
			case 'b':
				rv += '\b';
				break;
			case 'f':
				rv += '\f';
				break;
			case 'n':
				rv += '\n';
				break;
			case 'r':
				rv += '\r';
				break;
			case 't':
				rv += '\t';
				break;
			case 'u':
				AddUnicode(rv);
				break;
			default:
				throw std::runtime_error("invalid string");
			}
		} else if(static_cast<unsigned char>(ch) >= ' ') {
			rv += ch;
		} else {
			throw std::runtime_error("invalid string");
		}
	}
	return rv;
}

Value Parser::ParseValue() {
	auto const ch= Get();
	switch(ch) {
	case '[': // begin array
		return ParseArray();
	case '{': // begin object
		return ParseObject();
	case '"': // begin string
		return Value(ParseString());
	case 'f': // check for false
		if(Get() == 'a' && Get() == 'l' && Get() == 's' && Get() == 'e') {
			return Value(false);
		}
		break;
	case 'n': // check for null
		if(Get() == 'u' && Get() == 'l' && Get() == 'l') {
			return Value(nullptr);
		}
		break;
	case 't': // check for true
		if(Get() == 'r' && Get() == 'u' && Get() == 'e') {
			return Value(true);
		}
		break;
	default:
		if(isdigit(ch) || ch == '-') {
			return Value(ParseNumber(ch));
		}
	}
	throw std::runtime_error("invalid input");
}

void Parser::SkipWhitespace() noexcept {
	while(*next && (*next == '\t' || *next == '\n' || *next == '\r' || *next == ' ')) {
		++next;
	}
}

void WriteJson(std::ostream& os, nullptr_t) {
	os << "null";
}

void WriteJson(std::ostream& os, bool b) {
	auto const s= b ? "true" : "false";
	os << s;
}

void WriteJson(std::ostream& os, std::string const& s) {
	WriteJson(os, s.c_str());
}

void WriteJson(std::ostream& os, std::wstring const& s) {
	WriteJson(os, s.c_str());
}

inline char AsHexDigit(int value) noexcept {
	value &= 0x0f;
	return static_cast<char>(value < 10 ? value + '0' : value - 10 + 'A');
}

void WriteJson(std::ostream& os, char const* s) {
	if(s != nullptr) {
		os << '"';
		for(char ch; ch= *s, ch; ++s) {
			if(static_cast<unsigned char>(ch) < ' ') {
				if(ch == '\b') {
					os << "\\b";
				} else if(ch == '\t') {
					os << "\\t";
				} else if(ch == '\n') {
					os << "\\n";
				} else if(ch == '\f') {
					os << "\\f";
				} else if(ch == '\r') {
					os << "\\r";
				} else {
					os << "\\u00" << AsHexDigit(ch >> 4) << AsHexDigit(ch);
				}
			} else if(ch == '"' || ch == '\\') {
				os << '\\' << ch;
			} else {
				os << ch;
			}
		}
		os << '"';
	} else {
		WriteJson(os, nullptr);
	}
}

void WriteJson(std::ostream& os, wchar_t const* s) {
	if(s != nullptr) {
		std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
		auto const result= converter.to_bytes(s);
		WriteJson(os, result);
	} else {
		WriteJson(os, nullptr);
	}
}
