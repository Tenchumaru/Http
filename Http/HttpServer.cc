#include "pch.h"
#include "FibrousTcpSocketFactory.h"
#include "ClosableResponse.h"
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

namespace {
	std::array<char, 4> pattern = { '\r', '\n', '\r', '\n' };
	std::boyer_moore_searcher searcher(pattern.begin(), pattern.end());
}

TcpSocketFactory::fn_t HttpServer::GetConnectFn() const {
	return [this](TcpSocket&& socket) {
		for (;;) {
			// Read data until at least the end of the headers.
			std::array<char, 0x3800> buffer;
			auto* const begin = buffer.data();
			auto const m = buffer.size();
			auto* const end = begin + m;
			for (std::remove_const_t<decltype(m)> i = 0; i < m;) {
				auto const n = socket.Receive(begin + i, m - i);
				if (n <= 0) {
					return;
				}
				auto const* p = begin;
				i += n;
				for (;;) {
					auto* const next = begin + i;
					auto* const body = std::search(p, const_cast<decltype(p)>(next), searcher);
					if (body == next) {
						if (p != begin) {
							// This loop previously ran and processed at least one request.
							i = next - p;
							if (i) {
								memmove_s(begin, buffer.size(), p, i);
							}
						}
						break;
					}

					// Process the request.
					p = ProcessRequest(p, body + pattern.size(), next, end, socket);
					if (!p) {
						return;
					}
					if (p < begin || end < p) {
						// p actually points to a dynamically allocated std::vector<char>.
						auto const q = std::unique_ptr<std::vector<char> const>{ reinterpret_cast<std::vector<char> const*>(p) };
						if (buffer.size() < q->size()) {
							// There is too much data.  Close the socket.
							std::cerr << "too much data:  " << q->size() << std::endl;
							return;
						}
						std::copy(q->begin(), q->end(), begin);
						p = begin;
						i = q->size();
					}
				}
			}
		}
	};
}

char const* HttpServer::ProcessRequest(char const* begin, char const* body, char* next, char const* end, TcpSocket& socket) const {
	// begin through body contains the start line and headers.  body through end
	// contains none, some, or all of the body.  If it contains all of the body, it
	// is followed by zero or more full requests followed by nothing or a partial request.
	std::array<char, 0x3800> buffer;
	ClosableResponse response(socket, buffer.data(), buffer.data() + buffer.size());

	// Process the first request.  It's possible the process returns a dynamically
	// allocated buffer.  It's actually a std::vector<char>.  The process also checks
	// the Connection header value and returns nullptr to indicate closing the socket.
	try {
		end = DispatchRequest(begin, body, next, end, socket, response);
		response.Close();
	} catch (std::exception const& ex) {
		std::cerr << "DispatchRequest threw an exception:  " << ex.what() << std::endl;
		if (response.Reset()) {
			response.WriteStatus(StatusLines::InternalServerError);
			response.Close();
		}
		return nullptr;
	}

	// Return the end of the body.  If there are subsequent requests, the return
	// value will be the beginning of the next request.
	return end;
}
