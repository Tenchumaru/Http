#pragma once

class Socket {
public:
	explicit Socket(SOCKET socket);
	virtual ~Socket();

protected:
	SOCKET socket;
};
