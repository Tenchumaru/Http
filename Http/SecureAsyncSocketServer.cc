#include "pch.h"
#include "SecureAsyncSocketServer.h"
#include "SecureAsyncSocket.h"

void SecureAsyncSocketServer::ConfigureSecurity(char const* certificateChainFile, char const* privateKeyFile) {
	// Validate parameters.
	if (sslContext) {
		throw std::runtime_error("SecureAsyncSocketServer::ConfigureSecurity already invoked");
	}
	if (!certificateChainFile || !privateKeyFile) {
		throw std::runtime_error("SecureAsyncSocketServer::ConfigureSecurity null arguments");
	}

	// Initialize OpenSSL.
	SSL_load_error_strings();
	OpenSSL_add_ssl_algorithms();

	// Create the SSL context.
	sslContext.reset(SSL_CTX_new(TLS_server_method()));
	if (!sslContext) {
		perror("Cannot create SSL context");
		ERR_print_errors_fp(stderr);
		throw std::runtime_error("SecureAsyncSocketServer::ConfigureSecurity.SSL_CTX_new");
	}
	if (SSL_CTX_use_certificate_chain_file(sslContext.get(), certificateChainFile) <= 0) {
		ERR_print_errors_fp(stderr);
		throw std::runtime_error("SecureAsyncSocketServer::ConfigureSecurity.SSL_CTX_use_certificate_chain_file");
	}
	if (SSL_CTX_use_PrivateKey_file(sslContext.get(), privateKeyFile, SSL_FILETYPE_PEM) <= 0) {
		ERR_print_errors_fp(stderr);
		throw std::runtime_error("SecureAsyncSocketServer::ConfigureSecurity.SSL_CTX_use_PrivateKey_file");
	}
}

Task<std::pair<std::unique_ptr<AsyncSocket>, int>> SecureAsyncSocketServer::Accept(SOCKET serverSocket, socklen_t addressSize) {
	// Accept the socket.
	auto [clientSocket, errorCode] = co_await AsyncSocketServer::Accept(serverSocket, addressSize);

	// Check for an acceptance error.
	if (!errorCode) {
		if (!sslContext) {
			// This isn't a secure server.
			co_return{ std::move(clientSocket), 0 };
		}

		// Transform it into a secure socket.
		auto secureSocket = std::make_unique<SecureAsyncSocket>(std::move(*clientSocket), sslContext.get());

		// Perform SSL acceptance.
		errorCode = co_await secureSocket->Accept();
		if (!errorCode) {
			co_return{ std::move(secureSocket), 0 };
		}
	}
	co_return{ std::unique_ptr<AsyncSocket>{}, errorCode };
}
