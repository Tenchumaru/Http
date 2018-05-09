#include "stdafx.h"
#include "Waiter.h"

#ifdef _WIN32
static int poll(_Inout_ LPWSAPOLLFD fdArray, _In_ size_t fds, _In_ INT timeout) {
	return WSAPoll(fdArray, static_cast<ULONG>(fds), timeout);
}
#endif

static void set_nonblocking(SOCKET s) {
#ifdef _WIN32
	u_long value = 1;
	auto const result = ioctlsocket(s, FIONBIO, &value);
	if(result != 0) {
		std::cout << "set_nonblocking ioctlsocket failure: " << errno << std::endl;
		throw std::runtime_error("set_nonblocking ioctlsocket failure");
	}
#else
	auto result = fcntl(s, F_GETFL, 0);
	if(result >= 0) {
		result |= O_NONBLOCK;
		result = fcntl(s, F_SETFL, result);
	}
	if(result != 0) {
		perror("set_nonblocking fcntl failure");
		throw std::runtime_error("set_nonblocking fcntl failure");
	}
#endif
}

Waiter::Waiter() : current(&a), next(&b), entry(a.cend()) {}

Waiter::~Waiter() {}

void Waiter::Add(SOCKET s, short pollValue) {
	set_nonblocking(s);
	next->push_back({ s, pollValue });
}

SOCKET Waiter::Wait() {
	for(;;) {
		while(entry != current->cend()) {
			auto currentEntry = entry;
			++entry;
			if(currentEntry->revents) {
				return currentEntry->fd;
			} else {
				next->push_back(*currentEntry);
			}
		}
		std::swap(current, next);
		next->clear();
		if(current->empty()) {
			throw std::logic_error("Waiter::Wait");
		}
		check(poll(current->data(), current->size(), -1));
		entry = current->cbegin();
	}
}

void Waiter::Wait(fn_t onReady) {
	for(;;) {
		std::swap(current, next);
		next->clear();
		check(poll(current->data(), current->size(), -1));
		for(auto currentEntry = current->cbegin(); currentEntry != current->cend(); ++currentEntry) {
			if(currentEntry->revents) {
				onReady(currentEntry->fd);
			} else {
				next->push_back(*currentEntry);
			}
		}
	}
}
