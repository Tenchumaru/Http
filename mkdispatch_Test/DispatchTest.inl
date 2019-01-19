using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace mkdispatch_Test {
	TEST_CLASS(TestClassName) {

		TEST_METHOD_INITIALIZE(Initialize) {
			CollectParameter_invocationCount = 0;
			CollectQuery_succeeded = false;
			CollectQuery_failed = false;
			FourZeroFour_invoked = false;
			Pa_b_invoked = false;
			a_b_invoked = false;
			a_bc_invoked = false;
			root_invoked = false;
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
		}

public:
	TEST_METHOD(FourZeroFour1) {
		Dispatch("G /j\r");
		if(callsCollectParameter) {
			Assert::AreEqual(CollectParameter_invocationCount, 0ull);
		}
		Assert::IsFalse(CollectQuery_succeeded);
		if(callsCollectParameter) {
			Assert::IsTrue(CollectQuery_failed);
		} else {
			Assert::IsFalse(CollectQuery_failed);
		}
		Assert::IsTrue(FourZeroFour_invoked);
	}

	TEST_METHOD(FourZeroFour2) {
		Dispatch("O /j\r");
		if(callsCollectParameter) {
			Assert::AreEqual(CollectParameter_invocationCount, 0ull);
		}
		Assert::IsFalse(CollectQuery_succeeded);
		Assert::IsFalse(CollectQuery_failed);
		Assert::IsTrue(FourZeroFour_invoked);
	}

	TEST_METHOD(Pa_b) {
		Dispatch("P /a/b\r");
		if(callsCollectParameter) {
			Assert::AreEqual(CollectParameter_invocationCount, 0ull);
		}
		Assert::IsTrue(CollectQuery_succeeded);
		Assert::IsFalse(CollectQuery_failed);
		Assert::IsTrue(Pa_b_invoked);
	}

	TEST_METHOD(a_b) {
		Dispatch("G /a/b\r");
		if(callsCollectParameter) {
			Assert::AreEqual(CollectParameter_invocationCount, 0ull);
		}
		Assert::IsTrue(CollectQuery_succeeded);
		Assert::IsFalse(CollectQuery_failed);
		Assert::IsTrue(a_b_invoked);
	}

	TEST_METHOD(a_bc) {
		Dispatch("G /a/bc\r");
		if(callsCollectParameter) {
			Assert::AreEqual(CollectParameter_invocationCount, 0ull);
		}
		Assert::IsTrue(CollectQuery_succeeded);
		Assert::IsFalse(CollectQuery_failed);
		Assert::IsTrue(a_bc_invoked);
	}

	TEST_METHOD(root) {
		Dispatch("G /\r");
		if(callsCollectParameter) {
			Assert::AreEqual(CollectParameter_invocationCount, 0ull);
		}
		Assert::IsTrue(CollectQuery_succeeded);
		Assert::IsFalse(CollectQuery_failed);
		Assert::IsTrue(root_invoked);
	}

	TEST_METHOD(x_y) {
		Dispatch("G /x/y\r");
		if(callsCollectParameter) {
			Assert::AreEqual(CollectParameter_invocationCount, 0ull);
		}
		Assert::IsTrue(CollectQuery_succeeded);
		Assert::IsFalse(CollectQuery_failed);
		Assert::IsTrue(x_y_invoked);
	}

	TEST_METHOD(xy_z) {
		Dispatch("G /xy/z\r");
		if(callsCollectParameter) {
			Assert::AreEqual(CollectParameter_invocationCount, 0ull);
		}
		Assert::IsTrue(CollectQuery_succeeded);
		Assert::IsFalse(CollectQuery_failed);
		Assert::IsTrue(xy_z_invoked);
	}

	TEST_METHOD(r___p) {
		Dispatch("G /r/abc/p\r");
		if(callsCollectParameter) {
			Assert::AreEqual(CollectParameter_invocationCount, 1ull);
		}
		Assert::IsTrue(CollectQuery_succeeded);
		Assert::IsFalse(CollectQuery_failed);
		std::string s(r___p_p0, r___p_q0);
		Assert::AreEqual(std::string("abc"), s);
	}

	TEST_METHOD(q__) {
		Dispatch("G /q/abc\r");
		if(callsCollectParameter) {
			Assert::AreEqual(CollectParameter_invocationCount, 1ull);
		}
		Assert::IsTrue(CollectQuery_succeeded);
		Assert::IsFalse(CollectQuery_failed);
		std::string s(q___p0, q___q0);
		Assert::AreEqual(std::string("abc"), s);
	}

	TEST_METHOD(q____) {
		Dispatch("G /q/abc/xyz\r");
		if(callsCollectParameter) {
			Assert::AreEqual(CollectParameter_invocationCount, 2ull);
		}
		Assert::IsTrue(CollectQuery_succeeded);
		Assert::IsFalse(CollectQuery_failed);
		std::string s(q_____p0, q_____q0);
		Assert::AreEqual(std::string("abc"), s);
		std::string t(q_____p1, q_____q1);
		Assert::AreEqual(std::string("xyz"), t);
	}

	TEST_METHOD(z____y) {
		Dispatch("G /z/abc/xyz/y\r");
		if(callsCollectParameter) {
			Assert::AreEqual(CollectParameter_invocationCount, 2ull);
		}
		Assert::IsTrue(CollectQuery_succeeded);
		Assert::IsFalse(CollectQuery_failed);
		std::string s(z____y_p0, z____y_q0);
		Assert::AreEqual(std::string("abc"), s);
		std::string t(z____y_p1, z____y_q1);
		Assert::AreEqual(std::string("xyz"), t);
	}
	};
}
