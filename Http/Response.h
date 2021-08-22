#pragma once

#include <ClientSocket.h>
#include "xtypes.h"
#include "Date.h"

class HeaderBase;

class Response {
public:
	constexpr static ptrdiff_t MinimumBufferSize = 512;

	Response(Date const& date, ClientSocket& socket, char* begin, char* end);
	Response() = delete;
	Response(Response const&) = delete;
	Response(Response&& that) noexcept;
	Response& operator=(Response const&) = delete;
	Response& operator=(Response&&) noexcept = delete;
	bool HandleExpectation(HeaderBase& headers) { return outputStreamBuffer.HandleExpectation(headers); }
	void WriteStatusLine(std::string const& statusLine) { outputStreamBuffer.WriteStatusLine(statusLine); }
	void WriteHeader(std::string const& name, std::string const& value) {
		WriteHeader(xstring{ name.data(), name.data() + name.size() }, xstring{ value.data(), value.data() + value.size() });
	}
	void WriteHeader(std::string const& name, xstring const& value) {
		WriteHeader(xstring{ name.data(), name.data() + name.size() }, value);
	}
	void WriteHeader(xstring const& name, xstring const& value) { outputStreamBuffer.WriteHeader(name, value); }
	bool Reset() { return outputStreamBuffer.Reset(); }
	template<typename T>
	Response& operator<<(T t) {
		responseStream << t;
		return *this;
	}

protected:
	// These are protected since derived classes control the response lifetime.
	~Response() = default;
	void Close() { outputStreamBuffer.Close(); }

private:
	struct nstreambuf : public std::streambuf {
		nstreambuf(Date const& date, ClientSocket& socket, char* begin, char* end);
		nstreambuf() = delete;
		nstreambuf(nstreambuf const&) = delete;
		nstreambuf(nstreambuf&& that) noexcept;
		nstreambuf& operator=(nstreambuf const&) = delete;
		nstreambuf& operator=(nstreambuf&&) noexcept = delete;
		bool HandleExpectation(HeaderBase& headers);
		void WriteStatusLine(std::string const& statusLine);
		void WriteHeader(xstring const& name, xstring const& value);
		void Close();
		bool Reset();
		std::streamsize xsputn(char_type const* s, std::streamsize n) override;
		int overflow(int c) override;

	private:
		using WriteStatusLineFn = void(nstreambuf::*)(std::string const&);
		using WriteHeaderFn = void(nstreambuf::*)(xstring const& name, xstring const& value);
		using WriteBodyFn = void(nstreambuf::*)(char_type const* s, std::streamsize n);
		using WriteFn = void(nstreambuf::*)(char_type const* s, std::streamsize n);
		using CloseFn = void(nstreambuf::*)();
		using ResetFn = bool(nstreambuf::*)();

		struct StateFunctions {
			WriteStatusLineFn WriteStatusLine;
			WriteHeaderFn WriteHeader;
			WriteBodyFn WriteBody;
			WriteFn Write;
			CloseFn Close;
		};

		static std::array<StateFunctions, 5> states;
		Date const& date;
		ClientSocket& socket;
		char* begin{};
		char* next{};
		char* end{};
		std::optional<size_t> contentLength;
		size_t stateIndex{};
		bool wroteServer{};

		bool get_IsResettable() const;
		__declspec(property(get = get_IsResettable)) bool const IsResettable;
		void WriteBody(char_type const* s, std::streamsize n);
		void Write(char_type const* s, std::streamsize n);
		void CheckWriteHeader(xstring const& name, xstring const& value);
		void InternalWriteHeader(xstring const& name, xstring const& value);
		void WriteServerHeaders();
		void SendBuffer();
		void InternalReset();

		void InitialWriteStatusLine(std::string const& statusLine);
		void InitialWriteHeader(xstring const& name, xstring const& value);
		void InitialWriteBody(char_type const* s, std::streamsize n);
		void InitialWrite(char_type const* s, std::streamsize n);
		void InitialClose();

		void InHeadersWriteStatusLine(std::string const& statusLine);
		void InHeadersWriteHeader(xstring const& name, xstring const& value);
		void InHeadersWriteBody(char_type const* s, std::streamsize n);
		void InHeadersWrite(char_type const* s, std::streamsize n);
		void InHeadersClose();

		void SentSomeHeadersWriteStatusLine(std::string const& statusLine);
		void SentSomeHeadersWriteHeader(xstring const& name, xstring const& value);
		void SentSomeHeadersWriteBody(char_type const* s, std::streamsize n);
		void SentSomeHeadersWrite(char_type const* s, std::streamsize n);
		void SentSomeHeadersClose();

		void InChunkedBodyWriteStatusLine(std::string const& statusLine);
		void InChunkedBodyWriteHeader(xstring const& name, xstring const& value);
		void InChunkedBodyWriteBody(char_type const* s, std::streamsize n);
		void InChunkedBodyWrite(char_type const* s, std::streamsize n);
		void InChunkedBodyClose();

		void InBodyWriteStatusLine(std::string const& statusLine);
		void InBodyWriteHeader(xstring const& name, xstring const& value);
		void InBodyWriteBody(char_type const* s, std::streamsize n);
		void InBodyWrite(char_type const* s, std::streamsize n);
		void InBodyClose();
	};

	nstreambuf outputStreamBuffer;
	std::ostream responseStream;
};
