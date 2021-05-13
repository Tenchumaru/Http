#include "pch.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

#include "../Http/xtypes.h"
#include "../Http/QueryBase.h"
#include "../Http/HeaderBase.h"

namespace Microsoft {
	namespace VisualStudio {
		namespace CppUnitTestFramework {
			template<>
			inline std::wstring ToString<StatusLines::StatusLine>(StatusLines::StatusLine const& t) {
				std::wstringstream ss;
				if (t.first) {
					ss << t.first << ',' << t.second;
				} else {
					ss << "{}";
				}
				return ss.str();
			}
		}
	}
}

#include "Basic.inl"

namespace {
	class TestResponse : public Response {
	public:
		TestResponse() : Response(socket, buffer.data(), buffer.data() + buffer.size()) {}
		~TestResponse() = default;

	private:
		static std::array<char, Response::MinimumHeaderBufferSize> buffer;
		static TcpSocket socket;
	};

	std::array<char, Response::MinimumHeaderBufferSize> TestResponse::buffer;
	TcpSocket TestResponse::socket = TcpSocket(0);

	class Body {
	public:
		Body(char const* begin, char const* next, int size, TcpSocket& socket);
		Body(Body const&) = delete;
		Body(Body&&) = default;
		Body& operator=(Body const&) = delete;
		Body& operator=(Body&&) = default;
		~Body() = default;
		bool GetIsGood() const { return size == 4 && next - begin == 4 && !strncmp(begin, "body", 4); }
		__declspec(property(get = GetIsGood)) bool const IsGood;

	private:
		char const* begin;
		char const* next;
		int size;
		TcpSocket& socket;
	};

	Body::Body(char const* begin, char const* next, int size, TcpSocket& socket) : begin(begin), next(next), size(size), socket(socket) {}

	size_t CollectParameter_invocationCount;
	bool CollectQueries_succeeded;
	bool CollectQueries_failed;
	StatusLines::StatusLine FourExEx_invoked;
	bool root_invoked;
	bool a_b_invoked;
	bool a_bc_invoked;
	bool x_y_invoked;
	bool xy_z_invoked;
	char const* r___p_p0;
	char const* r___p_q0;
	char const* q_____p0;
	char const* q_____q0;
	char const* q_____p1;
	char const* q_____q1;
	char const* q___p0;
	char const* q___q0;
	char const* z____y_p0;
	char const* z____y_q0;
	char const* z____y_p1;
	char const* z____y_q1;
	bool m_m__invoked;
	char const* m_m___p0;
	char const* m_m___q0;
	char const* m_m___a_p0;
	char const* m_m___a_q0;
	char const* m_m___b_p0;
	char const* m_m___b_q0;
	bool Pa_b_hasGoodBody, Pa_b_invoked;
	bool Pa_b_c_hasGoodBody, Pa_b_c_invoked;
	bool Px___y_hasGoodBody;
	char const* Px___y_p0;
	char const* Px___y_q0;
	bool Px_y___z_hasGoodBody;
	char const* Px_y___z_p0;
	char const* Px_y___z_q0;

	bool AtEndOfPath(char ch) {
		return ch == '?' || ch == '#' || ch == ' ' || ch == '\r' || ch == '\n';
	}

	bool CollectParameter(char const*& p, size_t index, char const*& pn, char const*& qn) {
		++CollectParameter_invocationCount;
		pn = qn = p + index;
		while (*qn != '?' && *qn != '/' && *qn != '#' && *qn != ' ' && *qn != '\r' && *qn != '\n') {
			++qn;
		}
		if (pn == qn) {
			return false;
		}
		p = qn - index - 1;
		return true;
	}

	char const* CollectParameter(char const*& p, size_t index) {
		++CollectParameter_invocationCount;
		char const* q = p + index;
		while (*q != '?' && *q != '/' && *q != '#' && *q != ' ' && *q != '\r' && *q != '\n') {
			++q;
		}
		p = q - index;
		return q;
	}

	bool CollectQueries(char const* p) {
		CollectQueries_succeeded = AtEndOfPath(*p);
		CollectQueries_failed = !CollectQueries_succeeded;
		return CollectQueries_succeeded;
	}

	void FourExEx(Response& response, StatusLines::StatusLine const& statusLine, char const* message = nullptr) {
		response, message;
		FourExEx_invoked = statusLine;
	}
	void root(Query_one_ones__two&& queries, Header_contentXtype_customXheader_myXcustomXheader&& headers, Response& response) {
		queries, headers, response;
		root_invoked = true;
	}
	void a_b(Response& response) {
		response;
		a_b_invoked = true;
	}
	void a_bc(Response& response) {
		response;
		a_bc_invoked = true;
	}
	void x_y(Response& response) {
		response;
		x_y_invoked = true;
	}
	void xy_z(Response& response) {
		response;
		xy_z_invoked = true;
	}
	void r___p(char const* p0, char const* q0, Response& response) {
		response;
		r___p_p0 = p0;
		r___p_q0 = q0;
	}
	void q____(char const* p0, char const* q0, char const* p1, char const* q1, Response& response) {
		response;
		q_____p0 = p0;
		q_____q0 = q0;
		q_____p1 = p1;
		q_____q1 = q1;
	}
	void q__(char const* p0, char const* q0, Response& response) {
		response;
		q___p0 = p0;
		q___q0 = q0;
	}
	void z____y(char const* p0, char const* q0, char const* p1, char const* q1, Response& response) {
		response;
		z____y_p0 = p0;
		z____y_q0 = q0;
		z____y_p1 = p1;
		z____y_q1 = q1;
	}
	void m_m_(Response& response) {
		response;
		m_m__invoked = true;
	}
	void m_m__(char const* p0, char const* q0, Response& response) {
		response;
		m_m___p0 = p0;
		m_m___q0 = q0;
	}
	void m_m___a(char const* p0, char const* q0, Response& response) {
		response;
		m_m___a_p0 = p0;
		m_m___a_q0 = q0;
	}
	void m_m___b(char const* p0, char const* q0, Response& response) {
		response;
		m_m___b_p0 = p0;
		m_m___b_q0 = q0;
	}
	void Pa_b(Header_contentXlength&& headers, Body&& body, Response& response) {
		headers, body, response;
		Pa_b_hasGoodBody = body.IsGood;
		Pa_b_invoked = true;
	}
	void Pa_b_c(Header_contentXlength&& headers, Body&& body, Response& response) {
		headers, body, response;
		Pa_b_c_hasGoodBody = body.IsGood;
		Pa_b_c_invoked = true;
	}
	void Px___y(char const* p0, char const* q0, Header_contentXlength&& headers, Body&& body, Response& response) {
		headers, body, response;
		Px___y_hasGoodBody = body.IsGood;
		Px___y_p0 = p0;
		Px___y_q0 = q0;
	}
	void Px_y___z(char const* p0, char const* q0, Header_contentXlength&& headers, Body&& body, Response& response) {
		headers, body, response;
		Px_y___z_hasGoodBody = body.IsGood;
		Px_y___z_p0 = p0;
		Px_y___z_q0 = q0;
	}
}

#define DISPATCH
#include "Basic.inl"

namespace mkapp_Test {
	TEST_CLASS(MainTest) {

		TEST_METHOD_INITIALIZE(Initialize) {
			CollectParameter_invocationCount = 0;
			CollectQueries_succeeded = false;
			CollectQueries_failed = false;
			FourExEx_invoked = StatusLines::StatusLine{};
			root_invoked = false;
			a_b_invoked = false;
			a_bc_invoked = false;
			x_y_invoked = false;
			xy_z_invoked = false;
			r___p_p0 = nullptr;
			r___p_q0 = nullptr;
			q_____p0 = nullptr;
			q_____q0 = nullptr;
			q_____p1 = nullptr;
			q_____q1 = nullptr;
			q___p0 = nullptr;
			q___q0 = nullptr;
			z____y_p0 = nullptr;
			z____y_q0 = nullptr;
			z____y_p1 = nullptr;
			z____y_q1 = nullptr;
			m_m__invoked = false;
			m_m___p0 = nullptr;
			m_m___q0 = nullptr;
			m_m___a_p0 = nullptr;
			m_m___a_q0 = nullptr;
			m_m___b_p0 = nullptr;
			m_m___b_q0 = nullptr;
			Pa_b_invoked = false;
			Pa_b_c_invoked = false;
			Px___y_p0 = nullptr;
			Px___y_q0 = nullptr;
			Px_y___z_p0 = nullptr;
			Px_y___z_q0 = nullptr;
		}

public:
	TEST_METHOD(FourZeroFour1) {
		TcpSocket socket(0);
		TestResponse response;
		std::string request("G /j HTTP/1.1\r\n\r\n-");
		char* p = &request.back();
		Dispatch(request.c_str(), p, p, socket, response);
		Assert::AreEqual(CollectParameter_invocationCount, 0ull);
		Assert::IsFalse(CollectQueries_succeeded);
		Assert::IsFalse(CollectQueries_failed);
		Assert::AreEqual(StatusLines::NotFound, FourExEx_invoked);
	}

	TEST_METHOD(FourZeroFour2) {
		TcpSocket socket(0);
		TestResponse response;
		std::string request("O /j HTTP/1.1\r\n\r\n-");
		char* p = &request.back();
		Dispatch(request.c_str(), p, p, socket, response);
		Assert::AreEqual(CollectParameter_invocationCount, 0ull);
		Assert::IsFalse(CollectQueries_succeeded);
		Assert::IsFalse(CollectQueries_failed);
		Assert::AreEqual(StatusLines::NotFound, FourExEx_invoked);
	}

	TEST_METHOD(root) {
		TcpSocket socket(0);
		TestResponse response;
		std::string request("G / HTTP/1.1\r\n\r\n-");
		char* p = &request.back();
		Dispatch(request.c_str(), p, p, socket, response);
		Assert::AreEqual(CollectParameter_invocationCount, 0ull);
		Assert::IsTrue(root_invoked);
	}

	TEST_METHOD(a_b) {
		TcpSocket socket(0);
		TestResponse response;
		std::string request("G /a/b HTTP/1.1\r\n\r\n-");
		char* p = &request.back();
		Dispatch(request.c_str(), p, p, socket, response);
		Assert::AreEqual(CollectParameter_invocationCount, 0ull);
		Assert::IsTrue(a_b_invoked);
	}

	TEST_METHOD(a_bc) {
		TcpSocket socket(0);
		TestResponse response;
		std::string request("G /a/bc HTTP/1.1\r\n\r\n-");
		char* p = &request.back();
		Dispatch(request.c_str(), p, p, socket, response);
		Assert::AreEqual(CollectParameter_invocationCount, 0ull);
		Assert::IsTrue(a_bc_invoked);
	}

	TEST_METHOD(x_y) {
		TcpSocket socket(0);
		TestResponse response;
		std::string request("G /x/y HTTP/1.1\r\n\r\n-");
		char* p = &request.back();
		Dispatch(request.c_str(), p, p, socket, response);
		Assert::AreEqual(CollectParameter_invocationCount, 0ull);
		Assert::IsTrue(x_y_invoked);
	}

	TEST_METHOD(xy_z) {
		TcpSocket socket(0);
		TestResponse response;
		std::string request("G /xy/z HTTP/1.1\r\n\r\n-");
		char* p = &request.back();
		Dispatch(request.c_str(), p, p, socket, response);
		Assert::AreEqual(CollectParameter_invocationCount, 0ull);
		Assert::IsTrue(xy_z_invoked);
	}

	TEST_METHOD(r___p) {
		TcpSocket socket(0);
		TestResponse response;
		std::string request("G /r/abc/p HTTP/1.1\r\n\r\n-");
		char* p = &request.back();
		Dispatch(request.c_str(), p, p, socket, response);
		Assert::AreEqual(CollectParameter_invocationCount, 1ull);
		std::string s(r___p_p0, r___p_q0);
		Assert::AreEqual(std::string("abc"), s);
	}

	TEST_METHOD(q____) {
		TcpSocket socket(0);
		TestResponse response;
		std::string request("G /q/abc/xyz HTTP/1.1\r\n\r\n-");
		char* p = &request.back();
		Dispatch(request.c_str(), p, p, socket, response);
		Assert::AreEqual(CollectParameter_invocationCount, 2ull);
		std::string s(q_____p0, q_____q0);
		Assert::AreEqual(std::string("abc"), s);
		std::string t(q_____p1, q_____q1);
		Assert::AreEqual(std::string("xyz"), t);
	}

	TEST_METHOD(q__) {
		TcpSocket socket(0);
		TestResponse response;
		std::string request("G /q/abc HTTP/1.1\r\n\r\n-");
		char* p = &request.back();
		Dispatch(request.c_str(), p, p, socket, response);
		Assert::AreEqual(CollectParameter_invocationCount, 1ull);
		std::string s(q___p0, q___q0);
		Assert::AreEqual(std::string("abc"), s);
	}

	TEST_METHOD(z____y) {
		TcpSocket socket(0);
		TestResponse response;
		std::string request("G /z/abc/xyz/y HTTP/1.1\r\n\r\n-");
		char* p = &request.back();
		Dispatch(request.c_str(), p, p, socket, response);
		Assert::AreEqual(CollectParameter_invocationCount, 2ull);
		std::string s(z____y_p0, z____y_q0);
		Assert::AreEqual(std::string("abc"), s);
		std::string t(z____y_p1, z____y_q1);
		Assert::AreEqual(std::string("xyz"), t);
	}

	TEST_METHOD(m_m_) {
		TcpSocket socket(0);
		TestResponse response;
		std::string request("G /m/m/ HTTP/1.1\r\n\r\n-");
		char* p = &request.back();
		Dispatch(request.c_str(), p, p, socket, response);
		Assert::IsTrue(m_m__invoked);
	}

	TEST_METHOD(m_m__) {
		TcpSocket socket(0);
		TestResponse response;
		std::string request("G /m/m/abc HTTP/1.1\r\n\r\n-");
		char* p = &request.back();
		Dispatch(request.c_str(), p, p, socket, response);
		Assert::AreEqual(CollectParameter_invocationCount, 1ull);
		std::string s(m_m___p0, m_m___q0);
		Assert::AreEqual(std::string("abc"), s);
	}

	TEST_METHOD(m_m___a) {
		TcpSocket socket(0);
		TestResponse response;
		std::string request("G /m/m/abc/a HTTP/1.1\r\n\r\n-");
		char* p = &request.back();
		Dispatch(request.c_str(), p, p, socket, response);
		Assert::AreEqual(CollectParameter_invocationCount, 1ull);
		std::string s(m_m___a_p0, m_m___a_q0);
		Assert::AreEqual(std::string("abc"), s);
	}

	TEST_METHOD(m_m___b) {
		TcpSocket socket(0);
		TestResponse response;
		std::string request("G /m/m/abc/b HTTP/1.1\r\n\r\n-");
		char* p = &request.back();
		Dispatch(request.c_str(), p, p, socket, response);
		Assert::AreEqual(CollectParameter_invocationCount, 1ull);
		std::string s(m_m___b_p0, m_m___b_q0);
		Assert::AreEqual(std::string("abc"), s);
	}

	TEST_METHOD(Pa_b) {
		TcpSocket socket(0);
		TestResponse response;
		std::string request("P /a/b HTTP/1.1\r\nContent-Length: 4\r\n\r\nbody-");
		char* p = &request.back();
		Dispatch(request.c_str(), p - 4, p, socket, response);
		Assert::AreEqual(CollectParameter_invocationCount, 0ull);
		Assert::IsTrue(Pa_b_hasGoodBody);
		Assert::IsTrue(Pa_b_invoked);
	}

	TEST_METHOD(Pa_b_c) {
		TcpSocket socket(0);
		TestResponse response;
		std::string request("P /a/b/c HTTP/1.1\r\nContent-Length: 4\r\n\r\nbody-");
		char* p = &request.back();
		Dispatch(request.c_str(), p - 4, p, socket, response);
		Assert::AreEqual(CollectParameter_invocationCount, 0ull);
		Assert::IsTrue(Pa_b_c_hasGoodBody);
		Assert::IsTrue(Pa_b_c_invoked);
	}

	TEST_METHOD(Px___y) {
		TcpSocket socket(0);
		TestResponse response;
		std::string request("P /x/y/y HTTP/1.1\r\nContent-Length: 4\r\n\r\nbody-");
		char* p = &request.back();
		Dispatch(request.c_str(), p - 4, p, socket, response);
		Assert::AreEqual(CollectParameter_invocationCount, 2ull);
		Assert::IsTrue(Px___y_hasGoodBody);
		std::string s(Px___y_p0, Px___y_q0);
		Assert::AreEqual(std::string("y"), s);
	}

	TEST_METHOD(Px_y___z) {
		TcpSocket socket(0);
		TestResponse response;
		std::string request("P /x/y/y/z HTTP/1.1\r\nContent-Length: 4\r\n\r\nbody-");
		char* p = &request.back();
		Dispatch(request.c_str(), p - 4, p, socket, response);
		Assert::AreEqual(CollectParameter_invocationCount, 1ull);
		Assert::IsTrue(Px_y___z_hasGoodBody);
		std::string s(Px_y___z_p0, Px_y___z_q0);
		Assert::AreEqual(std::string("y"), s);
	}
	};
}
