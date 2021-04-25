#include "pch.h"
#include "SecureAsyncSocketServer.h"

using namespace std::literals;

void SecureAsyncSocketServer::ConfigureSecurity(char const* certificateChainFile, char const* privateKeyFile) {
	// Validate parameters.
	if(sslContext) {
		throw std::runtime_error("SecureAsyncSocketServer::ConfigureSecurity already invoked");
	}
	if(!certificateChainFile || !privateKeyFile) {
		throw std::runtime_error("SecureAsyncSocketServer::ConfigureSecurity null arguments");
	}

	// Initialize OpenSSL.
	SSL_load_error_strings();
	OpenSSL_add_ssl_algorithms();

	// Create the SSL context.
	sslContext.reset(SSL_CTX_new(TLS_server_method()));
	if(!sslContext) {
		perror("Cannot create SSL context");
		ERR_print_errors_fp(stderr);
		throw std::runtime_error("SecureAsyncSocketServer::ConfigureSecurity.SSL_CTX_new");
	}
	if(SSL_CTX_use_certificate_chain_file(sslContext.get(), certificateChainFile) <= 0) {
		ERR_print_errors_fp(stderr);
		throw std::runtime_error("SecureAsyncSocketServer::ConfigureSecurity.SSL_CTX_use_certificate_chain_file");
	}
	if(SSL_CTX_use_PrivateKey_file(sslContext.get(), privateKeyFile, SSL_FILETYPE_PEM) <= 0) {
		ERR_print_errors_fp(stderr);
		throw std::runtime_error("SecureAsyncSocketServer::ConfigureSecurity.SSL_CTX_use_PrivateKey_file");
	}
}

Task<std::pair<std::unique_ptr<AsyncSocket>, int>> SecureAsyncSocketServer::Accept(SOCKET serverSocket, socklen_t addressSize) {
	auto [clientSocket, errorCode] = co_await AsyncSocketServer::Accept(serverSocket, addressSize);
	// TODO
	co_return{ std::move(clientSocket), errorCode };
}
