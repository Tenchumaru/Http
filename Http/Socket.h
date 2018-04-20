#pragma once

class Socket {
public:
	explicit Socket(SOCKET socket);
	virtual ~Socket();
	void Close();

protected:
	SOCKET socket;
};
