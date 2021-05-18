#include "pch.h"
#include "Date.h"

using namespace std::chrono_literals;

Date::Date() : p(a), q(b), r(a) {
	static_assert(decltype(isShuttingDown)::is_always_lock_free);
	static_assert(decltype(p)::is_always_lock_free);
	static_assert(sizeof(a) == sizeof(b));
	task = std::async(std::launch::async, [this] { Tick(); });
}

Date::~Date() {
	isShuttingDown.store(true);
	cv.notify_one();
	task.wait();
}

void Date::Tick() {
	std::unique_lock<std::mutex> lock(mutex);
	do {
		time_t const t = time(nullptr);
#ifdef _WIN32
		tm m_;
		gmtime_s(&m_, &t);
		tm* const m = &m_;
#else
		tm* const m = gmtime(&t);
#endif
		n = std::strftime(q, sizeof(a), "%a, %d %b %Y %T GMT", m);
		p.store(q);
		std::swap(q, r);
	} while (!cv.wait_for(lock, 1s, [this] { return isShuttingDown.load(); }));
}
