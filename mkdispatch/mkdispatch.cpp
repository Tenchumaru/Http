#include "pch.h"

// This implementation has two faults:
// 1. It does not handle variables in the path (e.g. :clientId).
// 2. It is no better than the optimized implementations of memcmp.

struct Request {
	std::string verb;
	std::string url;
};

struct Node {
	std::unordered_map<uint64_t, Node> nodes;
	std::string line;
	size_t level;
};

using map = decltype(Node::nodes);

union Quad {
	struct {
		uint32_t word;
		uint32_t size;
	} parts;
	uint64_t whole;
};

namespace {
	Node root;

	void Parse(Request const& request) {
		std::string line = request.verb + ' ' + request.url;
		size_t i, size = line.size(), nwords = size / sizeof(uint32_t);
		auto* p = reinterpret_cast<uint32_t*>(&line[0]);
		auto* currentNode = &root;
		for(i = 0; i < nwords; ++i) {
			Quad match = { p[i], sizeof(uint32_t) };
			currentNode = &currentNode->nodes[match.whole];
			currentNode->level = i;
		}
		char const* end = &line.back();
		switch(size % 4) {
		case 1:
			currentNode = &currentNode->nodes[(Quad{ static_cast<uint32_t>(*end), 1 }).whole];
			currentNode->level = i;
			break;
		case 2:
			currentNode = &currentNode->nodes[(Quad{ *reinterpret_cast<uint16_t const*>(end - 1), 2 }).whole];
			currentNode->level = i;
			break;
		case 3:
			currentNode = &currentNode->nodes[(Quad{ 0x00ffffff & *reinterpret_cast<uint32_t const*>(end - 2), 3 }).whole];
			currentNode->level = i;
			break;
		}
		currentNode->line = line;
	}

	void Dump(Node const& node) {
		std::cout << node.level << ": ";
		if(!node.line.empty()) {
			std::cout << '"' << node.line;
			if(node.nodes.empty()) {
				std::cout << '"' << std::endl;
			} else {
				std::cout << "\" ";
			}
		}
		if(!node.nodes.empty()) {
			std::cout << '{' << std::endl;
			std::vector<std::pair<map::key_type, map::mapped_type>> v(node.nodes.cbegin(), node.nodes.cend());
			std::sort(v.begin(), v.end(), [](auto const& left, auto const& right) {
				return left.first < right.first;
			});
			std::for_each(v.crbegin(), v.crend(), [](auto const& pair) {
				std::cout << std::hex << pair.first << std::dec << ": ";
				Dump(pair.second);
			});
			std::cout << '}' << std::endl;
		}
	}

	void Print(Node const& node) {
		if(!node.nodes.empty()) {
			std::vector<std::pair<map::key_type, map::mapped_type>> v(node.nodes.cbegin(), node.nodes.cend());
			std::sort(v.begin(), v.end(), [](auto const& left, auto const& right) {
				return left.first < right.first;
			});
			std::for_each(v.crbegin(), v.crend(), [](auto const& pair) {
				std::cout << "\tif(";
				switch(pair.first >> 32) {
				case 1:
					std::cout << "*static_cast<uint8_t const*>(p + " << pair.second.level << ')';
					break;
				case 2:
					std::cout << "*static_cast<uint16_t const*>(p + " << pair.second.level << ')';
					break;
				case 3:
					std::cout << "p[" << pair.second.level << "] & 0x00ffffff";
					break;
				default:
					std::cout << "p[" << pair.second.level << ']';
					break;
				}
				std::cout << " == 0x" << std::hex << static_cast<uint32_t>(pair.first) << std::dec << ") {" << std::endl;
				Print(pair.second);
				std::cout << "\t}" << std::endl;
			});
		}
		if(!node.line.empty()) {
			std::cout << "\t// This is a terminating state for \"" << node.line << '"' << std::endl;
		}
	}
}

int main() {
	// TODO:  read the requests.
	std::vector<Request> requests = {
		{ "POST", "/extensions/:clientId/auth/secret" },
		{ "GET", "/extensions/:clientId/auth/secret" },
		{ "DELETE", "/extensions/:clientId/auth/secret" },
		{ "GET", "/extensions/:clientId/live_activated_channels" },
		{ "PUT", "/extensions/:clientId/:version/required_configuration" },
		{ "PUT", "/extensions/:clientId/configurations" },
		{ "GET", "/extensions/:clientId/configurations/channels/:channelId" },
		{ "GET", "/extensions/:clientId/configurations/segments/broadcaster" },
		{ "GET", "/extensions/:clientId/configurations/segments/developer" },
		{ "GET", "/extensions/:clientId/configurations/segments/global" },
		{ "POST", "/extensions/message/:channelId" },
		{ "POST", "/extensions/message/all" },
		{ "POST", "/extensions/:clientId/:version/channels/:channelId/chat" },
		{ "GET", "/helix/analytics/extensions" },
		{ "GET", "/helix/analytics/games" },
		{ "GET", "/helix/bits/leaderboard" },
		{ "POST", "/helix/clips" },
		{ "GET", "/helix/clips" },
		{ "POST", "/helix/entitlements/upload" },
		{ "GET", "/helix/entitlements/codes" },
		{ "POST", "/helix/entitlements/codes" },
		{ "GET", "/helix/games/top" },
		{ "GET", "/helix/games" },
		{ "GET", "/helix/streams" },
		{ "GET", "/helix/streams/metadata" },
		{ "POST", "/helix/streams/markers" },
		{ "GET", "/helix/streams/markers" },
		{ "GET", "/helix/users" },
		{ "GET", "/helix/users/follows" },
		{ "PUT", "/helix/users" },
		{ "GET", "/helix/users/extensions/list" },
		{ "GET", "/helix/users/extensions" },
		{ "PUT", "/helix/users/extensions" },
		{ "GET", "/helix/videos" },
		{ "GET", "/helix/webhooks/subscriptions" },
	};

	// Parse the requests into nodes.
	std::for_each(requests.cbegin(), requests.cend(), Parse);

	// TODO:  print the Dispatcher class.
	Dump(root);
	Print(root);
}
