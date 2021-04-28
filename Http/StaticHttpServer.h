#pragma once

#include "SecureAsyncSocketServer.h"

class StaticHttpServer : public SecureAsyncSocketServer {
public:
	StaticHttpServer() = default;
	StaticHttpServer(StaticHttpServer const&) = delete;
	StaticHttpServer(StaticHttpServer&&) noexcept = default;
	StaticHttpServer& operator=(StaticHttpServer const&) = delete;
	StaticHttpServer& operator=(StaticHttpServer&&) noexcept = default;
	~StaticHttpServer() override = default;
	Task<void> Handle(std::unique_ptr<AsyncSocket> clientSocket) override;
};
