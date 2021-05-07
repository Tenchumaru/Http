#include "pch.h"
#include "SecureFibrousTcpSocket.h"
#include "SecureFibrousSocketServer.h"
#ifdef _WIN32
using FiberFn = LPFIBER_START_ROUTINE;
#else
# include "Fiber.h"
#endif

namespace {
	constexpr size_t stackSize = 0x10000;

#ifdef _WIN32
	inline int poll(_Inout_ LPWSAPOLLFD fdArray, _In_ size_t fds, _In_ INT timeout) {
		return WSAPoll(fdArray, static_cast<ULONG>(fds), timeout);
	}
#endif
}

std::unique_ptr<TcpSocket> SecureFibrousSocketServer::MakeSocketImpl(SOCKET socket) const {
	auto socket_ = std::unique_ptr<FibrousTcpSocket>(static_cast<FibrousTcpSocket*>(FibrousSocketServer::MakeSocketImpl(socket).release()));
	return std::make_unique<SecureFibrousTcpSocket>(std::move(*socket_), sslContext.get(), false);
}

void SecureFibrousSocketServer::InternalHandle(std::unique_ptr<FibrousTcpSocket> clientSocket) {
	if (sslContext) {
		// Transform it into a secure socket.
		clientSocket = std::make_unique<SecureFibrousTcpSocket>(std::move(*clientSocket), sslContext.get(), true);
	}
	InternalHandleImpl(std::move(clientSocket));
}

void SecureFibrousSocketServer::ConfigureSecurity(char const* certificateChainFile, char const* privateKeyFile) {
	// Validate parameters.
	if (sslContext) {
		throw std::runtime_error("SecureFibrousSocketServer::ConfigureSecurity already invoked");
	}
	if (!certificateChainFile || !privateKeyFile) {
		throw std::runtime_error("SecureFibrousSocketServer::ConfigureSecurity null arguments");
	}

	// Initialize OpenSSL.
	SSL_load_error_strings();
	OpenSSL_add_ssl_algorithms();

	// Create the SSL context.
	sslContext.reset(SSL_CTX_new(TLS_server_method()));
	if (!sslContext) {
		perror("Cannot create SSL context");
		ERR_print_errors_fp(stderr);
		throw std::runtime_error("SecureFibrousSocketServer::ConfigureSecurity.SSL_CTX_new");
	}
	if (SSL_CTX_use_certificate_chain_file(sslContext.get(), certificateChainFile) <= 0) {
		ERR_print_errors_fp(stderr);
		throw std::runtime_error("SecureFibrousSocketServer::ConfigureSecurity.SSL_CTX_use_certificate_chain_file");
	}
	if (SSL_CTX_use_PrivateKey_file(sslContext.get(), privateKeyFile, SSL_FILETYPE_PEM) <= 0) {
		ERR_print_errors_fp(stderr);
		throw std::runtime_error("SecureFibrousSocketServer::ConfigureSecurity.SSL_CTX_use_PrivateKey_file");
	}
}
