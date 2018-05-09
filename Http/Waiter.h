#pragma once

class Waiter {
public:
	using fn_t = std::function<void(SOCKET)>;

	Waiter();
	~Waiter();
	void Add(SOCKET s, short pollValue);
	SOCKET Wait();
	void Wait(fn_t onReady);

private:
	std::vector<pollfd> a, b;
	std::vector<pollfd>* current;
	std::vector<pollfd>* next;
	std::vector<pollfd>::const_iterator entry;
};
