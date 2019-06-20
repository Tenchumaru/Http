#include "stdafx.h"
#include "SecureFibrousTcpSocket.h"

namespace {
	// TODO:  consider using thread-local storage for this.
	SSL_CTX* sslContext = [] {
		// Initialize OpenSSL.
		SSL_load_error_strings();
		OpenSSL_add_ssl_algorithms();

		// Create the SSL context.
		auto* const rv = SSL_CTX_new(TLS_server_method());
		if(!rv) {
			perror("Cannot create SSL context");
			ERR_print_errors_fp(stderr);
			exit(EXIT_FAILURE);
		}
		return rv;
	}();

#ifdef _DEBUG
	void PrintCommand(int command) {
		char const* s = "(unknown)";
		switch(command) {
		case BIO_CTRL_RESET:
			s = "CTRL_RESET";
			break;
		case BIO_C_FILE_SEEK:
			s = "C_FILE_SEEK";
			break;
		case BIO_C_FILE_TELL:
			s = "C_FILE_TELL";
			break;
		case BIO_CTRL_PUSH:
			s = "CTRL_PUSH";
			break;
		case BIO_CTRL_POP:
			s = "CTRL_POP";
			break;
		case BIO_CTRL_INFO:
			s = "CTRL_INFO";
			break;
		case BIO_CTRL_PENDING:
			s = "CTRL_PENDING";
			break;
		case BIO_CTRL_WPENDING:
			s = "CTRL_WPENDING";
			break;
		case BIO_CTRL_DUP:
			s = "CTRL_DUP";
			break;
		case BIO_CTRL_FLUSH:
			s = "CTRL_FLUSH";
			break;
		}
		OutputDebugStringA(s);
		OutputDebugStringA("\n");
	}
#else
# define PrintCommand(command) ((void)0)
#endif

	long BioControl(BIO* bio, int command, long value, void*) {
		PrintCommand(command);
		switch(command) {
		case BIO_CTRL_DUP:
		case BIO_CTRL_EOF:
		case BIO_CTRL_RESET:
			break;
		case BIO_CTRL_GET_CLOSE:
			return BIO_get_shutdown(bio);
		case BIO_CTRL_SET_CLOSE:
			BIO_set_shutdown(bio, static_cast<int>(value));
			return 1;
		case BIO_CTRL_INFO:
		case BIO_CTRL_PENDING:
		case BIO_CTRL_SET_CALLBACK:
		case BIO_CTRL_WPENDING:
			return 0;
		case BIO_CTRL_FLUSH:
		case BIO_CTRL_POP:
		case BIO_CTRL_PUSH:
			return 1;
		default:
			DebugBreak();
		}
		return -1;
	}

	int BioCreate(BIO* bio) {
		BIO_set_init(bio, 1);
		return 1;
	}

	int BioDestroy(BIO*) {
		return 1;
	}
}

BIO_METHOD* SecureFibrousTcpSocket::bioMethod = [] {
	auto* const rv = BIO_meth_new(BIO_get_new_index() | BIO_TYPE_SOURCE_SINK, "SecureFibrousTcpSocket::bioMethod");
	BIO_meth_set_ctrl(rv, BioControl);
	BIO_meth_set_create(rv, BioCreate);
	BIO_meth_set_destroy(rv, BioDestroy);
	BIO_meth_set_read_ex(rv, SecureFibrousTcpSocket::BioRead);
	BIO_meth_set_write_ex(rv, SecureFibrousTcpSocket::BioWrite);
	return rv;
}();

SecureFibrousTcpSocket::SecureFibrousTcpSocket(SOCKET socket, fn_t awaitFn, bool isServer) :
	FibrousTcpSocket(socket, awaitFn),
	ssl(SSL_new(sslContext)) {
	if(!ssl) {
		perror("Cannot create SSL");
		throw std::runtime_error("SecureFibrousTcpSocket::SecureFibrousTcpSocket.SSL_new");
	}
	auto* const bio = BIO_new(bioMethod);
	BIO_set_data(bio, this);
	SSL_set0_rbio(ssl, bio);
	BIO_up_ref(bio);
	SSL_set0_wbio(ssl, bio);
	if(isServer) {
		if(SSL_accept(ssl) <= 0) {
			ERR_print_errors_fp(stderr);
			throw std::runtime_error("SecureFibrousTcpSocket::SecureFibrousTcpSocket.SSL_accept");
		}
	} else {
		if(SSL_connect(ssl) <= 0) {
			ERR_print_errors_fp(stderr);
			throw std::runtime_error("SecureFibrousTcpSocket::SecureFibrousTcpSocket.SSL_connect");
		}
	}
}

SecureFibrousTcpSocket::~SecureFibrousTcpSocket() {
	SSL_free(ssl);
}

void SecureFibrousTcpSocket::Configure(char const* certificateChainFile, char const* privateKeyFile) {
	if(SSL_CTX_use_certificate_chain_file(sslContext, certificateChainFile) <= 0) {
		ERR_print_errors_fp(stderr);
		throw std::runtime_error("SecureFibrousTcpSocket::Configure.SSL_CTX_use_certificate_chain_file");
	}
	if(SSL_CTX_use_PrivateKey_file(sslContext, privateKeyFile, SSL_FILETYPE_PEM) <= 0) {
		ERR_print_errors_fp(stderr);
		throw std::runtime_error("SecureFibrousTcpSocket::Configure.SSL_CTX_use_PrivateKey_file");
	}
}

int SecureFibrousTcpSocket::InternalReceive(char* buffer, size_t bufferSize) {
	if(!bufferSize) {
		assert(bufferSize);
		return 0;
	}
	int result = SSL_read(ssl, buffer, static_cast<int>(bufferSize));
	if(result > 0) {
		return result;
	}
	result = SSL_get_error(ssl, result);
	return result == SSL_ERROR_ZERO_RETURN ? 0 : -1;
}

int SecureFibrousTcpSocket::InternalSend(char const* buffer, size_t bufferSize) {
	if(!bufferSize) {
		assert(bufferSize);
		return 0;
	}
	int result = SSL_write(ssl, buffer, static_cast<int>(bufferSize));
	if(result > 0) {
		return result;
	}
	result = SSL_get_error(ssl, result);
	return result == SSL_ERROR_ZERO_RETURN ? 0 : -1;
}

int SecureFibrousTcpSocket::BioRead(BIO* bio, char* data, size_t n, size_t* pn) {
	auto* const p = reinterpret_cast<SecureFibrousTcpSocket*>(BIO_get_data(bio));
	int const i = p->FibrousTcpSocket::InternalReceive(data, n);
	if(i < 0) {
		return -1;
	}
	*pn = i;
	return 1;
}

int SecureFibrousTcpSocket::BioWrite(BIO* bio, char const* data, size_t n, size_t* pn) {
	auto* const p = reinterpret_cast<SecureFibrousTcpSocket*>(BIO_get_data(bio));
	int const i = p->FibrousTcpSocket::InternalSend(data, n);
	if(i < 0) {
		return -1;
	}
	*pn = i;
	return 1;
}
