#pragma once

using xstring = std::pair<char const*, char const*>;

template<unsigned N>
struct xvector {
	xstring values[N];
	static constexpr unsigned max = N;
	unsigned count = 0;

	struct iterator {
		xvector const& reference;
		unsigned index = 0;
		bool operator!=(iterator const& that) const { return index != that.index; }
		void operator++() { ++index; }
		xstring const& operator*() const { return reference.values[index]; }
	};

	iterator begin() const { return iterator{ *this }; }
	iterator end() const { return iterator{ *this, count }; }
};
