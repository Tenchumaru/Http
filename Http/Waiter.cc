#include "stdafx.h"
#include "Waiter.h"

#ifdef _WIN32
static int poll(_Inout_ LPWSAPOLLFD fdArray, _In_ size_t fds, _In_ INT timeout) {
	return WSAPoll(fdArray, static_cast<ULONG>(fds), timeout);
}
#endif

void Waiter::Add(SOCKET s, short pollValue) {
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
