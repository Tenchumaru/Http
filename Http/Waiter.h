#pragma once

class Waiter {
public:
	using fn_t = std::function<void(SOCKET)>;

	Waiter() : current(&a), next(&b), entry(a.cend()) {}
	Waiter(Waiter const&) = delete;
	Waiter(Waiter&&) noexcept = default;
	Waiter& operator=(Waiter const&) = delete;
	Waiter& operator=(Waiter&&) noexcept = default;
	~Waiter() = default;

	void Add(SOCKET s, short pollValue);
	SOCKET Wait();
	void Wait(fn_t onReady);

private:
	std::vector<pollfd> a, b;
	std::vector<pollfd>* current;
	std::vector<pollfd>* next;
	std::vector<pollfd>::const_iterator entry;
};
