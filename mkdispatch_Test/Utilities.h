#include "../Http/Response.h"
#include "../Http/Http.h"

class TestSocket : public TcpSocket {
public:
	TestSocket() : TcpSocket(INVALID_SOCKET) {}
	~TestSocket() = default;
};

class TestResponse : public Response {
public:
	TestResponse() : Response(socket, buffer.data(), buffer.data() + buffer.size()) {}
	~TestResponse() = default;

private:
	std::array<char, MinimumBufferSize> buffer;
	TestSocket socket;
};

extern size_t CollectParameter_invocationCount;
extern bool CollectQueries_succeeded;
extern bool CollectQueries_failed;
extern std::string FourExEx_result;
extern bool root_invoked;
extern bool a_b_invoked;
extern bool a_bc_invoked;
extern bool x_y_invoked;
extern bool xy_z_invoked;
extern char const* r___p_p0;
extern char const* r___p_q0;
extern char const* q_____p0;
extern char const* q_____q0;
extern char const* q_____p1;
extern char const* q_____q1;
extern char const* q___p0;
extern char const* q___q0;
extern char const* z____y_p0;
extern char const* z____y_q0;
extern char const* z____y_p1;
extern char const* z____y_q1;
extern bool m_m__invoked;
extern char const* m_m___p0;
extern char const* m_m___q0;
extern char const* m_m___a_p0;
extern char const* m_m___a_q0;
extern char const* m_m___b_p0;
extern char const* m_m___b_q0;
extern bool Pa_b_invoked;
extern bool Pa_b_c_invoked;
extern char const* Px___y_p0;
extern char const* Px___y_q0;
extern char const* Px_y___z_p0;
extern char const* Px_y___z_q0;

char const* CollectParameter(char const*& p, size_t index);
bool CollectQueries(char const* p);

void root(Response& response);
void a_b(Response& response);
void a_bc(Response& response);
void x_y(Response& response);
void xy_z(Response& response);
void r___p(char const* p0, char const* q0, Response& response);
void q____(char const* p0, char const* q0, char const* p1, char const* q1, Response& response);
void q__(char const* p0, char const* q0, Response& response);
void z____y(char const* p0, char const* q0, char const* p1, char const* q1, Response& response);
void m_m_(Response& response);
void m_m__(char const* p0, char const* q0, Response& response);
void m_m___a(char const* p0, char const* q0, Response& response);
void m_m___b(char const* p0, char const* q0, Response& response);
void Pa_b(Response& response);
void Pa_b_c(Response& response);
void Px___y(char const* p0, char const* q0, Response& response);
void Px_y___z(char const* p0, char const* q0, Response& response);
