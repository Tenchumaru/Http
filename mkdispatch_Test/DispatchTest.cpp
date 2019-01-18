#include "stdafx.h"
#include "CppUnitTest.h"

namespace {
	bool CollectQuery_invoked;
	bool FourZeroFour_invoked;
	bool Pa_b_invoked;
	bool a_b_invoked;
	bool a_bc_invoked;
	bool root_invoked;
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

	bool CollectQuery(char const*) { return CollectQuery_invoked = true; }
	void FourZeroFour() { FourZeroFour_invoked = true; }
	void Pa_b() { Pa_b_invoked = true; }
	void a_b() { a_b_invoked = true; }
	void a_bc() { a_bc_invoked = true; }
	void root() { root_invoked = true; }
	void x_y() { x_y_invoked = true; }
	void xy_z() { xy_z_invoked = true; }
	void r___p(char const* p0, char const* q0) { r___p_p0 = p0; r___p_q0 = q0; }
	void q__(char const* p0, char const* q0) { q___p0 = p0; q___q0 = q0; }
	void q____(char const* p0, char const* q0, char const* p1, char const* q1) { q_____p0 = p0; q_____q0 = q0; q_____p1 = p1; q_____q1 = q1; }
}

#include "Dispatch.inl"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace mkdispatch_Test {
	TEST_CLASS(UnitTest1) {

		TEST_METHOD_INITIALIZE(Initialize) {
			CollectQuery_invoked = false;
			FourZeroFour_invoked = false;
			Pa_b_invoked = false;
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
		}

public:
	TEST_METHOD(FourZeroFour) {
		Dispatch("G /z\r");
		Assert::IsFalse(CollectQuery_invoked);
		Assert::IsTrue(FourZeroFour_invoked);
	}

	TEST_METHOD(Pa_b) {
		Dispatch("P /a/b\r");
		Assert::IsTrue(CollectQuery_invoked);
		Assert::IsTrue(Pa_b_invoked);
	}

	TEST_METHOD(a_b) {
		Dispatch("G /a/b\r");
		Assert::IsTrue(CollectQuery_invoked);
		Assert::IsTrue(a_b_invoked);
	}

	TEST_METHOD(a_bc) {
		Dispatch("G /a/bc\r");
		Assert::IsTrue(CollectQuery_invoked);
		Assert::IsTrue(a_bc_invoked);
	}

	TEST_METHOD(x_y) {
		Dispatch("G /x/y\r");
		Assert::IsTrue(CollectQuery_invoked);
		Assert::IsTrue(x_y_invoked);
	}

	TEST_METHOD(xy_z) {
		Dispatch("G /xy/z\r");
		Assert::IsTrue(CollectQuery_invoked);
		Assert::IsTrue(xy_z_invoked);
	}

	TEST_METHOD(r___p) {
		Dispatch("G /r/abc/p\r");
		Assert::IsTrue(CollectQuery_invoked);
		std::string s(r___p_p0, r___p_q0);
		Assert::AreEqual(std::string("abc"), s);
	}

	TEST_METHOD(q__) {
		Dispatch("G /q/abc\r");
		Assert::IsTrue(CollectQuery_invoked);
		std::string s(q___p0, q___q0);
		Assert::AreEqual(std::string("abc"), s);
	}

	TEST_METHOD(q____) {
		Dispatch("G /q/abc/xyz\r");
		Assert::IsTrue(CollectQuery_invoked);
		std::string s(q_____p0, q_____q0);
		Assert::AreEqual(std::string("abc"), s);
		std::string t(q_____p1, q_____q1);
		Assert::AreEqual(std::string("xyz"), t);
	}
	};
}
