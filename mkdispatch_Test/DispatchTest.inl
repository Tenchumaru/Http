using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace mkdispatch_Test {
	TEST_CLASS(TestClassName) {

		TEST_METHOD_INITIALIZE(Initialize) {
			CollectParameter_invocationCount = 0;
			CollectQueries_succeeded = false;
			CollectQueries_failed = false;
			FourHundred_invoked = false;
			FourZeroFour_invoked = false;
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
		TcpSocket socket;
		Response response;
		Dispatch("G /j HTTP/1.1\r\n\r\n", nullptr, nullptr, socket, response);
		if (callsCollectParameter) {
			Assert::AreEqual(CollectParameter_invocationCount, 0ull);
		}
		Assert::IsFalse(CollectQueries_succeeded);
		if (callsCollectQueries) {
			Assert::IsTrue(CollectQueries_failed);
		} else {
			Assert::IsFalse(CollectQueries_failed);
		}
		Assert::IsTrue(FourZeroFour_invoked);
	}

	TEST_METHOD(FourZeroFour2) {
		TcpSocket socket;
		Response response;
		Dispatch("O /j HTTP/1.1\r\n\r\n", nullptr, nullptr, socket, response);
		if (callsCollectParameter) {
			Assert::AreEqual(CollectParameter_invocationCount, 0ull);
		}
		if (callsCollectQueries) {
			Assert::IsFalse(CollectQueries_succeeded);
			Assert::IsFalse(CollectQueries_failed);
		}
		Assert::IsTrue(FourZeroFour_invoked);
	}

	TEST_METHOD(root) {
		TcpSocket socket;
		Response response;
		Dispatch("G / HTTP/1.1\r\n\r\n", nullptr, nullptr, socket, response);
		if (callsCollectParameter) {
			Assert::AreEqual(CollectParameter_invocationCount, 0ull);
		}
		if (callsCollectQueries) {
			Assert::IsTrue(CollectQueries_succeeded);
			Assert::IsFalse(CollectQueries_failed);
		}
		Assert::IsTrue(root_invoked);
	}

	TEST_METHOD(a_b) {
		TcpSocket socket;
		Response response;
		Dispatch("G /a/b HTTP/1.1\r\n\r\n", nullptr, nullptr, socket, response);
		if (callsCollectParameter) {
			Assert::AreEqual(CollectParameter_invocationCount, 0ull);
		}
		if (callsCollectQueries) {
			Assert::IsTrue(CollectQueries_succeeded);
			Assert::IsFalse(CollectQueries_failed);
		}
		Assert::IsTrue(a_b_invoked);
	}

	TEST_METHOD(a_bc) {
		TcpSocket socket;
		Response response;
		Dispatch("G /a/bc HTTP/1.1\r\n\r\n", nullptr, nullptr, socket, response);
		if (callsCollectParameter) {
			Assert::AreEqual(CollectParameter_invocationCount, 0ull);
		}
		if (callsCollectQueries) {
			Assert::IsTrue(CollectQueries_succeeded);
			Assert::IsFalse(CollectQueries_failed);
		}
		Assert::IsTrue(a_bc_invoked);
	}

	TEST_METHOD(x_y) {
		TcpSocket socket;
		Response response;
		Dispatch("G /x/y HTTP/1.1\r\n\r\n", nullptr, nullptr, socket, response);
		if (callsCollectParameter) {
			Assert::AreEqual(CollectParameter_invocationCount, 0ull);
		}
		if (callsCollectQueries) {
			Assert::IsTrue(CollectQueries_succeeded);
			Assert::IsFalse(CollectQueries_failed);
		}
		Assert::IsTrue(x_y_invoked);
	}

	TEST_METHOD(xy_z) {
		TcpSocket socket;
		Response response;
		Dispatch("G /xy/z HTTP/1.1\r\n\r\n", nullptr, nullptr, socket, response);
		if (callsCollectParameter) {
			Assert::AreEqual(CollectParameter_invocationCount, 0ull);
		}
		if (callsCollectQueries) {
			Assert::IsTrue(CollectQueries_succeeded);
			Assert::IsFalse(CollectQueries_failed);
		}
		Assert::IsTrue(xy_z_invoked);
	}

	TEST_METHOD(r___p) {
		TcpSocket socket;
		Response response;
		Dispatch("G /r/abc/p HTTP/1.1\r\n\r\n", nullptr, nullptr, socket, response);
		if (callsCollectParameter) {
			Assert::AreEqual(CollectParameter_invocationCount, 1ull);
		}
		if (callsCollectQueries) {
			Assert::IsTrue(CollectQueries_succeeded);
			Assert::IsFalse(CollectQueries_failed);
		}
		std::string s(r___p_p0, r___p_q0);
		Assert::AreEqual(std::string("abc"), s);
	}

	TEST_METHOD(q____) {
		TcpSocket socket;
		Response response;
		Dispatch("G /q/abc/xyz HTTP/1.1\r\n\r\n", nullptr, nullptr, socket, response);
		if (callsCollectParameter) {
			Assert::AreEqual(CollectParameter_invocationCount, 2ull);
		}
		if (callsCollectQueries) {
			Assert::IsTrue(CollectQueries_succeeded);
			Assert::IsFalse(CollectQueries_failed);
		}
		std::string s(q_____p0, q_____q0);
		Assert::AreEqual(std::string("abc"), s);
		std::string t(q_____p1, q_____q1);
		Assert::AreEqual(std::string("xyz"), t);
	}

	TEST_METHOD(q__) {
		TcpSocket socket;
		Response response;
		Dispatch("G /q/abc HTTP/1.1\r\n\r\n", nullptr, nullptr, socket, response);
		if (callsCollectParameter) {
			Assert::AreEqual(CollectParameter_invocationCount, 1ull);
		}
		if (callsCollectQueries) {
			Assert::IsTrue(CollectQueries_succeeded);
			Assert::IsFalse(CollectQueries_failed);
		}
		std::string s(q___p0, q___q0);
		Assert::AreEqual(std::string("abc"), s);
	}

	TEST_METHOD(z____y) {
		TcpSocket socket;
		Response response;
		Dispatch("G /z/abc/xyz/y HTTP/1.1\r\n\r\n", nullptr, nullptr, socket, response);
		if (callsCollectParameter) {
			Assert::AreEqual(CollectParameter_invocationCount, 2ull);
		}
		if (callsCollectQueries) {
			Assert::IsTrue(CollectQueries_succeeded);
			Assert::IsFalse(CollectQueries_failed);
		}
		std::string s(z____y_p0, z____y_q0);
		Assert::AreEqual(std::string("abc"), s);
		std::string t(z____y_p1, z____y_q1);
		Assert::AreEqual(std::string("xyz"), t);
	}

	TEST_METHOD(m_m_) {
		TcpSocket socket;
		Response response;
		Dispatch("G /m/m/ HTTP/1.1\r\n\r\n", nullptr, nullptr, socket, response);
		if (callsCollectQueries) {
			Assert::IsTrue(CollectQueries_succeeded);
			Assert::IsFalse(CollectQueries_failed);
		}
		Assert::IsTrue(m_m__invoked);
	}

	TEST_METHOD(m_m__) {
		TcpSocket socket;
		Response response;
		Dispatch("G /m/m/abc HTTP/1.1\r\n\r\n", nullptr, nullptr, socket, response);
		if (callsCollectParameter) {
			Assert::AreEqual(CollectParameter_invocationCount, 1ull);
		}
		if (callsCollectQueries) {
			Assert::IsTrue(CollectQueries_succeeded);
			Assert::IsFalse(CollectQueries_failed);
		}
		std::string s(m_m___p0, m_m___q0);
		Assert::AreEqual(std::string("abc"), s);
	}

	TEST_METHOD(m_m___a) {
		TcpSocket socket;
		Response response;
		Dispatch("G /m/m/abc/a HTTP/1.1\r\n\r\n", nullptr, nullptr, socket, response);
		if (callsCollectParameter) {
			Assert::AreEqual(CollectParameter_invocationCount, 1ull);
		}
		if (callsCollectQueries) {
			Assert::IsTrue(CollectQueries_succeeded);
			Assert::IsFalse(CollectQueries_failed);
		}
		std::string s(m_m___a_p0, m_m___a_q0);
		Assert::AreEqual(std::string("abc"), s);
	}

	TEST_METHOD(m_m___b) {
		TcpSocket socket;
		Response response;
		Dispatch("G /m/m/abc/b HTTP/1.1\r\n\r\n", nullptr, nullptr, socket, response);
		if (callsCollectParameter) {
			Assert::AreEqual(CollectParameter_invocationCount, 1ull);
		}
		if (callsCollectQueries) {
			Assert::IsTrue(CollectQueries_succeeded);
			Assert::IsFalse(CollectQueries_failed);
		}
		std::string s(m_m___b_p0, m_m___b_q0);
		Assert::AreEqual(std::string("abc"), s);
	}

	TEST_METHOD(Pa_b) {
		TcpSocket socket;
		Response response;
		Dispatch("P /a/b HTTP/1.1\r\n\r\n", nullptr, nullptr, socket, response);
		if (callsCollectParameter) {
			Assert::AreEqual(CollectParameter_invocationCount, 0ull);
		}
		if (callsCollectQueries) {
			Assert::IsTrue(CollectQueries_succeeded);
			Assert::IsFalse(CollectQueries_failed);
		}
		Assert::IsTrue(Pa_b_invoked);
	}

	TEST_METHOD(Pa_b_c) {
		TcpSocket socket;
		Response response;
		Dispatch("P /a/b/c HTTP/1.1\r\n\r\n", nullptr, nullptr, socket, response);
		if (callsCollectParameter) {
			Assert::AreEqual(CollectParameter_invocationCount, 0ull);
		}
		if (callsCollectQueries) {
			Assert::IsTrue(CollectQueries_succeeded);
			Assert::IsFalse(CollectQueries_failed);
		}
		Assert::IsTrue(Pa_b_c_invoked);
	}

	TEST_METHOD(Px___y) {
		TcpSocket socket;
		Response response;
		Dispatch("P /x/y/y HTTP/1.1\r\n\r\n", nullptr, nullptr, socket, response);
		if (callsCollectParameter) {
			Assert::AreEqual(CollectParameter_invocationCount, 2ull);
		}
		if (callsCollectQueries) {
			Assert::IsTrue(CollectQueries_succeeded);
			Assert::IsFalse(CollectQueries_failed);
		}
		std::string s(Px___y_p0, Px___y_q0);
		Assert::AreEqual(std::string("y"), s);
	}

	TEST_METHOD(Px_y___z) {
		TcpSocket socket;
		Response response;
		Dispatch("P /x/y/y/z HTTP/1.1\r\n\r\n", nullptr, nullptr, socket, response);
		if (callsCollectParameter) {
			Assert::AreEqual(CollectParameter_invocationCount, 1ull);
		}
		if (callsCollectQueries) {
			Assert::IsTrue(CollectQueries_succeeded);
			Assert::IsFalse(CollectQueries_failed);
		}
		std::string s(Px_y___z_p0, Px_y___z_q0);
		Assert::AreEqual(std::string("y"), s);
	}
	};
}
