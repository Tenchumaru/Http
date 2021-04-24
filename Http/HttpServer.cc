#include "pch.h"
#include "FibrousTcpSocketFactory.h"
#include "HttpServer.h"

// TODO:  consider an alternate design in which the Request object produced by
// a RequestParser has its handler invoked on a new fiber so the parser can
// continue, possibly parsing and producing the next request if the first
// becomes blocked.  However, since it's all the same socket, the client will
// expect the responses in the same order as the requests.  Buffer out-of-order
// responses until the preceding responses are sent.  Additionally, if a later
// request depends on the outcome of an earlier request, this will necessitate
// synchronization to ensure effects occur in the expected order.

void HttpServer::ConfigureSecurity(char const* certificateChainFile_, char const* privateKeyFile_) {
	// Validate parameters.
	if (!certificateChainFile_ || !privateKeyFile_) {
		throw std::runtime_error("HttpServer::ConfigureSecurity null arguments");
	}

	// Save parameter values.
	certificateChainFile = certificateChainFile_;
	privateKeyFile = privateKeyFile_;
}

void HttpServer::Run(unsigned short port) {
	FibrousTcpSocketFactory server;
	if (!certificateChainFile.empty()) {
		server.ConfigureSecurity(certificateChainFile.c_str(), privateKeyFile.c_str());
	}
	server.CreateServer(port, GetConnectFn());
}
