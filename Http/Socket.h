#pragma once

class Socket {
public:
	explicit Socket(SOCKET socket) : socket(socket) {}
	Socket() = delete;
	Socket(Socket const&) = delete;
	Socket(Socket&&) = default;
	Socket& operator=(Socket const&) = delete;
	Socket& operator=(Socket&&) = default;
	virtual ~Socket();

	void Close();

protected:
	SOCKET socket;
};
