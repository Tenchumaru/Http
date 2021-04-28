#pragma once

#include "SecureAsyncSocketServer.h"
#include "Request.h"
#include "AsyncResponse.h"

class DynamicHttpServer : public SecureAsyncSocketServer {
public:
	using fn_t = std::function<Task<void>(Request const&, AsyncResponse&)>;

	DynamicHttpServer() = default;
	DynamicHttpServer(DynamicHttpServer const&) = delete;
	DynamicHttpServer(DynamicHttpServer&&) noexcept = default;
	DynamicHttpServer& operator=(DynamicHttpServer const&) = delete;
	DynamicHttpServer& operator=(DynamicHttpServer&&) noexcept = default;
	~DynamicHttpServer() = default;
	void Add(char const* path, fn_t fn);
	Task<void> Handle(std::unique_ptr<AsyncSocket> clientSocket) override;

private:
	std::vector<std::pair<std::string, fn_t>> handlers;

	Task<bool> InternalHandle(Request const&, std::unique_ptr<AsyncSocket>& clientSocket) const;
};
