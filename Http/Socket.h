#pragma once

class Socket {
public:
	explicit Socket(SOCKET socket) : socket(socket) {}
	Socket() = delete;
	Socket(Socket const&) = delete;
	Socket(Socket&& that) noexcept;
	Socket& operator=(Socket const&) = delete;
	Socket& operator=(Socket&& that) noexcept;
	virtual ~Socket();
	void Close();
	static bool SetNonblocking(SOCKET socket);

protected:
	SOCKET socket;
};
