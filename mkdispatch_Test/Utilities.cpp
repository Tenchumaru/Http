#include "pch.h"
#include "Utilities.h"

size_t CollectParameter_invocationCount;
bool CollectQueries_succeeded;
bool CollectQueries_failed;
std::string FourExEx_result;
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
bool Pa_b_invoked;
bool Pa_b_c_invoked;
char const* Px___y_p0;
char const* Px___y_q0;
char const* Px_y___z_p0;
char const* Px_y___z_q0;

// This is necessary to prevent linking with the implementation in the Http project.
bool AtEndOfPath(char ch) {
	return ch == '?' || ch == '#' || ch == ' ' || ch == '\r' || ch == '\n';
}

// This is necessary to prevent linking with the implementation in the Http project.
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

void FourExEx(Response&, std::string const& statusLine, char const*) {
	FourExEx_result = statusLine;
}

void root(Response& response) { response; root_invoked = true; }
void a_b(Response& response) { response; a_b_invoked = true; }
void a_bc(Response& response) { response; a_bc_invoked = true; }
void x_y(Response& response) { response; x_y_invoked = true; }
void xy_z(Response& response) { response; xy_z_invoked = true; }
void r___p(char const* p0, char const* q0, Response& response) { response; r___p_p0 = p0; r___p_q0 = q0; }
void q____(char const* p0, char const* q0, char const* p1, char const* q1, Response& response) { response; q_____p0 = p0; q_____q0 = q0; q_____p1 = p1; q_____q1 = q1; }
void q__(char const* p0, char const* q0, Response& response) { response; q___p0 = p0; q___q0 = q0; }
void z____y(char const* p0, char const* q0, char const* p1, char const* q1, Response& response) { response; z____y_p0 = p0; z____y_q0 = q0; z____y_p1 = p1; z____y_q1 = q1; }
void m_m_(Response& response) { response; m_m__invoked = true; }
void m_m__(char const* p0, char const* q0, Response& response) { response; m_m___p0 = p0; m_m___q0 = q0; }
void m_m___a(char const* p0, char const* q0, Response& response) { response; m_m___a_p0 = p0; m_m___a_q0 = q0; }
void m_m___b(char const* p0, char const* q0, Response& response) { response; m_m___b_p0 = p0; m_m___b_q0 = q0; }
void Pa_b(Response& response) { response; Pa_b_invoked = true; }
void Pa_b_c(Response& response) { response; Pa_b_c_invoked = true; }
void Px___y(char const* p0, char const* q0, Response& response) { response; Px___y_p0 = p0; Px___y_q0 = q0; }
void Px_y___z(char const* p0, char const* q0, Response& response) { response; Px_y___z_p0 = p0; Px_y___z_q0 = q0; }
