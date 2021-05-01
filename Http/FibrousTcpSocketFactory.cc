#include "pch.h"
#include "FibrousTcpSocketFactory.h"
#include "SecureFibrousTcpSocket.h"
#include "Waiter.h"
#ifdef _WIN32
using FiberFn = LPFIBER_START_ROUTINE;
#else
# include "Fiber.h"
#endif

namespace {
	constexpr size_t stackSize = 0x10000;
}

void FibrousTcpSocketFactory::ConfigureSecurity(char const* certificateChainFile, char const* privateKeyFile) {
	// Validate parameters.
	if (sslContext) {
		throw std::runtime_error("FibrousTcpSocketFactory::ConfigureSecurity already invoked");
	}
	if (!certificateChainFile || !privateKeyFile) {
		throw std::runtime_error("FibrousTcpSocketFactory::ConfigureSecurity null arguments");
	}

	// Initialize OpenSSL.
	SSL_load_error_strings();
	OpenSSL_add_ssl_algorithms();

	// Create the SSL context.
	sslContext.reset(SSL_CTX_new(TLS_server_method()));
	if (!sslContext) {
		perror("Cannot create SSL context");
		ERR_print_errors_fp(stderr);
		throw std::runtime_error("FibrousTcpSocketFactory::ConfigureSecurity.SSL_CTX_new");
	}
	if (SSL_CTX_use_certificate_chain_file(sslContext.get(), certificateChainFile) <= 0) {
		ERR_print_errors_fp(stderr);
		throw std::runtime_error("FibrousTcpSocketFactory::ConfigureSecurity.SSL_CTX_use_certificate_chain_file");
	}
	if (SSL_CTX_use_PrivateKey_file(sslContext.get(), privateKeyFile, SSL_FILETYPE_PEM) <= 0) {
		ERR_print_errors_fp(stderr);
		throw std::runtime_error("FibrousTcpSocketFactory::ConfigureSecurity.SSL_CTX_use_PrivateKey_file");
	}

	// Set the connection invocation function to the secure version.
	invokeOnConnectFn = [this] { return onConnect(SecureFibrousTcpSocket(socket, awaitFn, sslContext.get(), true)); };
}

void FibrousTcpSocketFactory::Accept(SOCKET server, socklen_t addressSize, fn_t onConnect_) {
	// Enable the server socket for asynchronous connections.
	if (!Socket::SetNonblocking(server)) {
		throw std::runtime_error("FibrousTcpSocketFactory::Accept.Socket::SetNonblocking");
	}
	onConnect = onConnect_;
	Waiter waiter;
	waiter.Add(server, POLLIN);

	// Initialize for coroutines.
	mainFiber = ConvertThreadToFiber(this);
	if (!mainFiber) {
		throw std::runtime_error("ConvertThreadToFiber");
	}

	// Create the await function.
	std::unordered_map<SOCKET, void*> fiberMap;
	awaitFn = [mainFiber = mainFiber, &waiter, &fiberMap](SOCKET s, short pollValue) {
		waiter.Add(s, pollValue);
		fiberMap[s] = GetCurrentFiber();
		SwitchToFiber(mainFiber);
		fiberMap.erase(s);
	};

	try {
		// Await connections and requests.
		SOCKADDR_STORAGE address;
		auto* p = reinterpret_cast<sockaddr*>(&address);
		for (;;) {
			SOCKET ready = waiter.Wait();
			if (ready == server) {
				socket = accept(server, p, &addressSize);
				if (socket == INVALID_SOCKET) {
					// Assume the failure is due to the network infrastructure
					// rejecting the connection.
					std::cerr << "failed connection: " << errno << std::endl;
					continue;
				}
#ifdef _DEBUG
				char node[NI_MAXHOST], service[NI_MAXSERV];
				if (getnameinfo(p, addressSize, node, sizeof(node), service, sizeof(service), NI_NUMERICHOST | NI_NUMERICSERV) == 0) {
					std::cout << "new connection: " << socket << '@' << node << ':' << service << std::endl;
				} else {
					std::cerr << "cannot get name information for accepted socket of family " << address.ss_family << std::endl;
				}
#endif
				if (!Socket::SetNonblocking(socket)) {
					close(socket);
					continue;
				}

				// Create a fiber and invoke the handler on it.
				void* fiber;
				if (availableFibers.empty()) {
					fiber = CreateFiber(stackSize, &FibrousTcpSocketFactory::InvokeOnConnect, this);
				} else {
					fiber = availableFibers.back();
					availableFibers.pop_back();
				}
				SwitchToFiber(fiber);
				waiter.Add(server, POLLIN);
			} else {
				auto it = fiberMap.find(ready);
				if (it != fiberMap.cend()) {
					auto fiber = it->second;
					fiberMap.erase(it);
					SwitchToFiber(fiber);
				} else {
					std::cerr << "warning: abandoned socket" << std::endl;
				}
			}
		}
	} catch (std::exception& ex) {
		std::cerr << "exception: " << ex.what() << std::endl;
	}

	// Destroy the fibers, close the server socket, and exit.
	std::for_each(availableFibers.cbegin(), availableFibers.cend(), [](void* fiber) { DeleteFiber(fiber); });
	close(server);
	if (!ConvertFiberToThread()) {
		throw std::runtime_error("ConvertFiberToThread");
	}
}

void FibrousTcpSocketFactory::InvokeOnConnect(void* parameter) {
	auto* p = reinterpret_cast<FibrousTcpSocketFactory*>(parameter);
	for (;;) {
		try {
			p->invokeOnConnectFn();
		} catch (std::exception const& ex) {
			std::cout << "FibrousTcpSocketFactory::InvokeOnConnect:  " << ex.what() << std::endl;
		}
		p->availableFibers.push_back(GetCurrentFiber());
		SwitchToFiber(p->mainFiber);
	}
}
