#include "pch.h"

// This implementation does not handle variables in the path (e.g. :clientId).

using vector = std::vector<std::string>;

struct Node {
	std::unordered_map<char, Node> nodes;
	std::string line;
	std::string prefix;
	size_t index;
};

using map = decltype(Node::nodes);

namespace {
	char constexpr flag = '\x1e';

	Node root;

	void InternalParse(std::string const& line, size_t index, Node& node) {
		// If the node is empty (i.e., has neither nodes nor a prefix), set its
		// line and prefix.
		auto& prefix = node.prefix;
		auto suffix = line.substr(index);
		if(node.nodes.empty() && prefix.empty()) {
			node.line = line;
			std::swap(prefix, suffix);
			node.index = index;
		} else {
			// Find the index of the character at which the suffix and prefix diverge.
			auto pair = std::mismatch(prefix.cbegin(), prefix.cend(), suffix.cbegin(), suffix.cend());

			// If the suffix does not start with the prefix, split the node
			// into two nodes, one with the common part of the prefix and
			// another with the rest of it.  The latter node will contain the
			// current node's nodes and the former will contain the latter.
			if(pair.first != prefix.cend()) {
				size_t divergenceIndex = pair.first - prefix.cbegin();
				/*
				before:
				node {
					nodes: {N},
					line: S + A + "bC",
					prefix: A + "bC",
					index: S.size(),
				}

				after adding suffix A + "xY" of line S + A + "xY":
				node {
					nodes: {
						'b': {
							nodes: {N},
							line: S + A + "bC",
							prefix: "C",
							index: S.size() + A.size() + 1,
						},
						'x': {
							nodes: {},
							line: S + A + "xY",
							prefix: "Y",
							index: S.size() + A.size() + 1,
						},
					},
					line: "",
					prefix: A,
					index: S.size(),
				}
				*/
				node.nodes = { { *pair.first, { node.nodes, node.line, prefix.substr(divergenceIndex + 1), node.index + divergenceIndex + 1 } } };
				node.line.clear();
				prefix.erase(divergenceIndex);
				index += divergenceIndex;
				suffix.erase(0, divergenceIndex);
			} else if(prefix.size()) {
				// The suffix starts with the prefix.  Remove the latter from
				// the former and adjust the index.
				suffix.erase(0, prefix.size());
				index += prefix.size();
			}

			// If the suffix is empty, replace this node's line.
			if(suffix.empty()) {
				if(!node.line.empty()) {
					// Warn about replacement.
					std::cout << "warning: replacing \"" << node.line << "\" with \"" << line << '"' << std::endl;
				}
				node.line = line;
			} else {
				// Otherwise, recurse into the child node.
				auto& child = node.nodes[suffix[0]];
				InternalParse(line, index + 1, child);
			}
		}
	}

	void Parse(vector const& lines) {
		std::for_each(lines.cbegin(), lines.cend(), [](std::string const& line) {
			std::string processedLine;
			std::string::size_type i = 0, j;
			while(j = line.find(':', i), j != std::string::npos) {
				processedLine.append(line.cbegin() + i, line.cbegin() + j);
				processedLine += flag;
				i = line.find('/', j);
				if(i == std::string::npos) {
					i = line.size();
				}
			}
			processedLine.append(line.substr(i));
			InternalParse(processedLine, 0, root);
		});
	}

	void Dump(Node const& node, int level = 0) {
		auto const& prefix = node.prefix;
		if(!prefix.empty()) {
			std::cout << '"' << prefix << "\" ";
		}
		std::cout << '(' << node.index << "): ";
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
			std::for_each(v.crbegin(), v.crend(), [level = level + 1](auto const& pair) {
				std::cout << std::string(level, ' ') << pair.first << ": ";
				Dump(pair.second, level);
			});
			std::cout << std::string(level, ' ') << '}' << std::endl;
		}
	}

	void Print(Node const& node, int level = 1) {
		auto tabs = std::string(level, '\t');
		if(!node.nodes.empty()) {
			std::cout << tabs << "switch(p[" << (node.index + node.prefix.size()) << "]) {" << std::endl;
			std::for_each(node.nodes.cbegin(), node.nodes.cend(), [level, &tabs](map::value_type const& pair) {
				std::cout << tabs << "case '" << pair.first << "':";
				if(!pair.second.prefix.empty()) {
					std::cout << " if(memcmp(p + " << (pair.second.index) << ", \"";
					std::cout << pair.second.prefix << "\", " << pair.second.prefix.size() << ") == 0)";
				}
				std::cout << " {" << std::endl;
				Print(pair.second, level + 1);
				std::cout << tabs << "\t} break;" << std::endl;
			});
			std::cout << tabs << "}" << std::endl;
		}
		if(!node.line.empty()) {
			std::cout << tabs << "if(p[" << (node.index + node.prefix.size());
			std::cout << "] == '\\r') return doit(\"" << node.line << "\");" << std::endl;
		}
	}
}

int main() {
	// TODO:  read the requests.
	std::vector<std::string> requests = {
		"POST /extensions/:clientId/auth/secret",
		"GET /extensions/:clientId/auth/secret",
		"DELETE /extensions/:clientId/auth/secret",
		"GET /extensions/:clientId/live_activated_channels",
		"PUT /extensions/:clientId/:version/required_configuration",
		"PUT /extensions/:clientId/configurations",
		"GET /extensions/:clientId/configurations/channels/:channelId",
		"GET /extensions/:clientId/configurations/segments/broadcaster",
		"GET /extensions/:clientId/configurations/segments/developer",
		"GET /extensions/:clientId/configurations/segments/global",
		"POST /extensions/message/:channelId",
		"POST /extensions/message/all",
		"POST /extensions/:clientId/:version/channels/:channelId/chat",
		"GET /helix/analytics/extensions",
		"GET /helix/analytics/games",
		"GET /helix/bits/leaderboard",
		"POST /helix/clips",
		"GET /helix/clips",
		"POST /helix/entitlements/upload",
		"GET /helix/entitlements/codes",
		"POST /helix/entitlements/codes",
		"GET /helix/games/top",
		"GET /helix/games",
		"GET /helix/streams",
		"GET /helix/streams/metadata",
		"POST /helix/streams/markers",
		"GET /helix/streams/markers",
		"GET /helix/users",
		"GET /helix/users/follows",
		"PUT /helix/users",
		"GET /helix/users/extensions/list",
		"GET /helix/users/extensions",
		"PUT /helix/users/extensions",
		"GET /helix/videos",
		"GET /helix/webhooks/subscriptions",
	};

	// Parse the requests into nodes.
	Parse(requests);

	// TODO:  print the Dispatcher class.
	Dump(root);
	Print(root);
}
