#pragma once

class Waiter {
public:
	using fn_t= std::function<void(SOCKET)>;

	Waiter();
	~Waiter();
	void Add(SOCKET s, short pollValue);
	void Remove(SOCKET s);
	SOCKET Wait();
	void Wait(fn_t onReady);

private:
	std::unordered_map<SOCKET, std::vector<pollfd>::size_type> indexMap;
	std::vector<pollfd> fds;
	std::vector<pollfd>::size_type current;
};
