#pragma once

#include "xtypes.h"

class Date {
public:
	Date();
	Date(Date const&) = delete;
	Date& operator=(Date const&) = delete;
	virtual ~Date();
	xstring operator()() const {
		auto const* p_ = p.load();
		return{ p_, p_ + n };
	}

private:
	std::future<void> task;
	std::atomic_bool isShuttingDown;
	std::mutex mutex;
	std::condition_variable cv;
	std::atomic<char*> p;
	char* q{};
	char* r{};
	size_t n{};
	char a[33];
	char b[33];

	void SetTime();
	void Tick();
};
