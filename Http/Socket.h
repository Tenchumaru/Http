#pragma once

class Socket {
public:
	explicit Socket(SOCKET socket);
	~Socket();
	size_t Receive(char const* buffer, size_t bufferSize);

private:
	SOCKET socket;
};
