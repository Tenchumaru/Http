#include "stdafx.h"
#include "Waiter.h"

#ifdef _WIN32
static int poll(_Inout_ LPWSAPOLLFD fdArray, _In_ size_t fds, _In_ INT timeout) {
	return WSAPoll(fdArray, static_cast<ULONG>(fds), timeout);
}
#endif

static void set_nonblocking(SOCKET s) {
#ifdef _WIN32
	u_long value= 1;
	auto const result= ioctlsocket(s, FIONBIO, &value);
	if(result != 0) {
		std::cout << "set_nonblocking ioctlsocket failure: " << errno << std::endl;
		throw std::runtime_error("set_nonblocking ioctlsocket failure");
	}
#else
	auto result= fcntl(s, F_GETFL, 0);
	if(result >= 0) {
		result |= O_NONBLOCK;
		result= fcntl(s, F_SETFL, result);
	}
	if(result != 0) {
		perror("set_nonblocking fcntl failure");
		throw std::runtime_error("set_nonblocking fcntl failure");
	}
#endif
}

Waiter::Waiter() : current(fds.size()) {}

Waiter::~Waiter() {}

void Waiter::Add(SOCKET s, short pollValue) {
	auto const it= indexMap.find(s);
	if(it == indexMap.end()) {
		set_nonblocking(s);
		bool atEnd= current == fds.size();
		indexMap.insert({ s, fds.size() });
		fds.push_back({ s, pollValue });
		if(atEnd) {
			current= fds.size();
		}
	} else {
		fds[it->second].events= pollValue;
	}
}

void Waiter::Remove(SOCKET s) {
	auto const it= indexMap.find(s);
	if(it != indexMap.end()) {
		auto const index= it->second;
		indexMap.erase(it);
		if(index == current) {
			// This is safe so long as the first descriptor is never removed.
			--current;
		}
		auto const last= fds.back().fd;
		auto const position= fds.begin() + index;
		std::swap(*position, fds.back());
		indexMap[last]= index;
		fds.pop_back();
	}
}

SOCKET Waiter::Wait() {
	for(;;) {
		if(fds.empty()) {
			return INVALID_SOCKET;
		}
		if(current == fds.size()) {
			current= 0;
			check(poll(fds.data(), fds.size(), -1));
		}
		while(current != fds.size()) {
			auto it= fds.begin() + current++;
			if(it->revents & (POLLIN | POLLOUT)) {
				return it->fd;
			} else if(it->revents & (POLLNVAL | POLLHUP)) {
				// Remove the socket from the collection.
				std::cout << "connection " << it->fd << " closing" << std::endl;
				Remove(it->fd);

				// TODO:  If there is a coroutine for the socket, shut it down.
			} else if(it->revents) {
				std::cout << "unexpected event: " << it->revents << std::endl;
			}
		}
	}
}

void Waiter::Wait(fn_t onReady) {
	check(poll(fds.data(), fds.size(), -1));
	for(current= 0; current != fds.size(); ++current) {
			auto it= fds.begin() + current;
		if(it->revents & (POLLIN | POLLOUT)) {
			onReady(it->fd);
		} else if(it->revents & (POLLNVAL | POLLHUP)) {
			// Remove the socket from the collection.
			std::cout << "connection " << it->fd << " closing" << std::endl;
			Remove(it->fd);

			// TODO:  If there is a coroutine for the socket, shut it down.
		} else if(it->revents) {
			std::cout << "unexpected event: " << it->revents << std::endl;
		}
	}
}
