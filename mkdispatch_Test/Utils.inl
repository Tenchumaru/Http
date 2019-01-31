namespace {
	size_t CollectParameter_invocationCount;
	bool CollectQuery_succeeded;
	bool CollectQuery_failed;
	bool FourZeroFour_invoked;
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
	bool Pa_b_invoked;
	char const* Px___y_p0;
	char const* Px___y_q0;
	char const* Px_y___z_p0;
	char const* Px_y___z_q0;

	bool CollectParameter(char const*& p, size_t index, char const*& pn, char const*& qn) {
		++CollectParameter_invocationCount;
		pn = qn = p + index;
		while(*qn != '?' && *qn != '/' && *qn != '\r' && *qn != '\n') {
			++qn;
		}
		p = qn - index - 1;
		return true;
	}

	char const* CollectParameter(char const*& p, size_t index) {
		++CollectParameter_invocationCount;
		char const* q = p + index;
		while(*q != '?' && *q != '/' && *q != '\r' && *q != '\n') {
			++q;
		}
		p = q - index;
		return q;
	}

	bool CollectQuery(char const* p) { 
		CollectQuery_succeeded = *p == '?' || *p == '\r' || *p == '\n';
		CollectQuery_failed = !CollectQuery_succeeded;
		return CollectQuery_succeeded;
	}

	void FourZeroFour() { FourZeroFour_invoked = true; }
	void root() { root_invoked = true; }
	void a_b() { a_b_invoked = true; }
	void a_bc() { a_bc_invoked = true; }
	void x_y() { x_y_invoked = true; }
	void xy_z() { xy_z_invoked = true; }
	void r___p(char const* p0, char const* q0) { r___p_p0 = p0; r___p_q0 = q0; }
	void q____(char const* p0, char const* q0, char const* p1, char const* q1) { q_____p0 = p0; q_____q0 = q0; q_____p1 = p1; q_____q1 = q1; }
	void q__(char const* p0, char const* q0) { q___p0 = p0; q___q0 = q0; }
	void z____y(char const* p0, char const* q0, char const* p1, char const* q1) { z____y_p0 = p0; z____y_q0 = q0; z____y_p1 = p1; z____y_q1 = q1; }
	void Pa_b() { Pa_b_invoked = true; }
	void Px___y(char const* p0, char const* q0) { Px___y_p0 = p0; Px___y_q0 = q0; }
	void Px_y___z(char const* p0, char const* q0) { Px_y___z_p0 = p0; Px_y___z_q0 = q0; }
}
