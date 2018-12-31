#include "pch.h"

struct Request {
	std::string line;
	std::string fn;
};

using vector = std::vector<Request>;

struct Node {
	std::map<char, Node> nodes;
	std::string fn;
	std::string prefix;
	size_t index;
};

using map = decltype(Node::nodes);

namespace {
	char constexpr flag = '\x1e';

	Node root;

	void InternalParse(std::string const& line, std::string const& fn, size_t index, Node& node) {
		// If the node is empty (i.e., has neither nodes nor a prefix), set its
		// line and prefix.
		auto& prefix = node.prefix;
		auto suffix = line.substr(index);
		if(node.nodes.empty() && prefix.empty()) {
			node.fn = fn;
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
				node.nodes = { { *pair.first, { node.nodes, node.fn, prefix.substr(divergenceIndex + 1), node.index + divergenceIndex + 1 } } };
				node.fn.clear();
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
				if(!node.fn.empty()) {
					// Warn about replacement.
					std::cout << "warning: replacing \"" << node.fn << "\" with \"" << fn << '"' << std::endl;
				}
				node.fn = fn;
			} else {
				// Otherwise, recurse into the child node.
				auto& child = node.nodes[suffix[0]];
				InternalParse(line, fn, index + 1, child);
			}
		}
	}

	void ReplaceParameter(Node& node) {
		/*
		before:
		node {
			nodes: {N},
			line: S + A + flag + B,
			prefix: A + flag + B,
			index: S.size(),
		}

		after:
		node {
			nodes: {
				flag: {
					nodes: {N},
					line: S + A + flag + B,
					prefix: B,
					index: S.size() + A.size() + 1,
				},
			},
			line: "",
			prefix: A,
			index: S.size(),
		}
		*/
		auto i = node.prefix.find(flag);
		if(i != std::string::npos) {
			node.nodes = { { flag, { node.nodes, node.fn, node.prefix.substr(i + 1), node.index + i + 1 } } };
			node.fn.clear();
			node.prefix.erase(i);
		}
		std::for_each(node.nodes.begin(), node.nodes.end(), [](map::value_type& pair) { ReplaceParameter(pair.second); });
	}

	void Parse(vector const& requests) {
		// Add each line, with parameter names removed, to the node structure.
		std::for_each(requests.cbegin(), requests.cend(), [](Request const& request) {
			std::string processedLine;
			std::string::size_type i = 0, j;
			size_t nparameters = 0;
			while(j = request.line.find(':', i), j != std::string::npos) {
				++nparameters;
				processedLine.append(request.line.cbegin() + i, request.line.cbegin() + j);
				processedLine += flag;
				i = request.line.find('/', j);
				if(i == std::string::npos) {
					i = request.line.size();
				}
			}
			processedLine.append(request.line.substr(i));
			std::ostringstream fn;
			fn << request.fn << '(';
#pragma warning(disable: 4456)
			for(size_t i = 0; i < nparameters; ++i) {
				fn << 'p' << i;
				if(i + 1 < nparameters) {
					fn << ", ";
				}
			}
#pragma warning(default: 4456)
			fn << ')';
			InternalParse(processedLine, fn.str(), 0, root);
		});

		// Replace any parameters in prefixes with an additional node.  This
		// will allow the printing logic to represent them properly.
		ReplaceParameter(root);
	}

	void Dump(Node const& node, int level = 0) {
		auto const& prefix = node.prefix;
		if(!prefix.empty()) {
			std::cout << '"' << prefix << "\" ";
		}
		std::cout << '(' << node.index << "): ";
		if(!node.fn.empty()) {
			std::cout << '"' << node.fn;
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

	void Print(Node const& node, int level, int parameterIndex);

	void PrintCompare(map::value_type const& pair, int level, int parameterIndex) {
		auto tabs = std::string(level, '\t');
		if(!pair.second.prefix.empty()) {
			std::cout << tabs;
			if(pair.second.prefix.size() == 1) {
				std::cout << "if(p[" << pair.second.index << "] == '" << pair.second.prefix << "') {" << std::endl;
			} else {
				std::cout << "if(memcmp(p + " << pair.second.index << ", \"";
				std::cout << pair.second.prefix << "\", " << pair.second.prefix.size() << ") == 0) {" << std::endl;
			}
			Print(pair.second, level + 1, parameterIndex);
			std::cout << tabs << '}' << std::endl;
		} else {
			Print(pair.second, level, parameterIndex);
		}
	}

	void Print(Node const& node, int level, int parameterIndex) {
		auto tabs = std::string(level, '\t');
		if(!node.nodes.empty()) {
			if(node.nodes.size() >= 2 || node.nodes.cbegin()->first != flag) {
				std::cout << tabs << "switch(p[" << (node.index + node.prefix.size()) << "]) {" << std::endl;
				std::for_each(node.nodes.crbegin(), node.nodes.crend(), [level, parameterIndex, &tabs](map::value_type const& pair) {
					if(pair.first == flag) {
						return;
					}
					std::cout << tabs << "case '" << pair.first << "':" << std::endl;
					PrintCompare(pair, level + 1, parameterIndex);
					std::cout << tabs << "\tbreak;" << std::endl;
				});
				std::cout << tabs << '}' << std::endl;
			}
			auto const& pair = *node.nodes.cbegin();
			if(pair.first == flag) {
				std::cout << tabs << "char const* p" << parameterIndex << " = collect_parameter(p, " << (pair.second.index) << ");" << std::endl;
				PrintCompare(pair, level, parameterIndex + 1);
			}
		}
		if(!node.fn.empty()) {
			std::cout << tabs << "if(p[" << (node.index + node.prefix.size());
			std::cout << "] == '\\r') return " << node.fn << ';' << std::endl;
		}
	}

	void Print(Node const& node) {
		std::cout << "void Dispatch(char const* p) {" << std::endl;
		Print(node, 1, 0);
		std::cout << "\treturn FourZeroFour();" << std::endl;
		std::cout << '}' << std::endl;
	}
}

int main() {
	// TODO:  read the requests.
	vector requests = {
		{ "POST /extensions/:clientId/auth/secret", "POST_extensions__clientId_auth_secret" },
		{ "GET /extensions/:clientId/auth/secret", "GET_extensions__clientId_auth_secret" },
		{ "DELETE /extensions/:clientId/auth/secret", "DELETE_extensions__clientId_auth_secret" },
		{ "GET /extensions/:clientId/live_activated_channels", "GET_extensions__clientId_live_activated_channels" },
		{ "PUT /extensions/:clientId/:version/required_configuration", "PUT_extensions__clientId__version_required_configuration" },
		{ "PUT /extensions/:clientId/configurations", "PUT_extensions__clientId_configurations" },
		{ "GET /extensions/:clientId/configurations/channels/:channelId", "GET_extensions__clientId_configurations_channels__channelId" },
		{ "GET /extensions/:clientId/configurations/segments/broadcaster", "GET_extensions__clientId_configurations_segments_broadcaster" },
		{ "GET /extensions/:clientId/configurations/segments/developer", "GET_extensions__clientId_configurations_segments_developer" },
		{ "GET /extensions/:clientId/configurations/segments/global", "GET_extensions__clientId_configurations_segments_global" },
		{ "POST /extensions/message/:channelId", "POST_extensions_message__channelId" },
		{ "POST /extensions/message/all", "POST_extensions_message_all" },
		{ "POST /extensions/:clientId/:version/channels/:channelId/chat", "POST_extensions__clientId__version_channels__channelId_chat" },
		{ "GET /helix/analytics/extensions", "GET_helix_analytics_extensions" },
		{ "GET /helix/analytics/games", "GET_helix_analytics_games" },
		{ "GET /helix/bits/leaderboard", "GET_helix_bits_leaderboard" },
		{ "POST /helix/clips", "POST_helix_clips" },
		{ "GET /helix/clips", "GET_helix_clips" },
		{ "POST /helix/entitlements/upload", "POST_helix_entitlements_upload" },
		{ "GET /helix/entitlements/codes", "GET_helix_entitlements_codes" },
		{ "POST /helix/entitlements/codes", "POST_helix_entitlements_codes" },
		{ "GET /helix/games/top", "GET_helix_games_top" },
		{ "GET /helix/games", "GET_helix_games" },
		{ "GET /helix/streams", "GET_helix_streams" },
		{ "GET /helix/streams/metadata", "GET_helix_streams_metadata" },
		{ "POST /helix/streams/markers", "POST_helix_streams_markers" },
		{ "GET /helix/streams/markers", "GET_helix_streams_markers" },
		{ "GET /helix/users", "GET_helix_users" },
		{ "GET /helix/users/follows", "GET_helix_users_follows" },
		{ "PUT /helix/users", "PUT_helix_users" },
		{ "GET /helix/users/extensions/list", "GET_helix_users_extensions_list" },
		{ "GET /helix/users/extensions", "GET_helix_users_extensions" },
		{ "PUT /helix/users/extensions", "PUT_helix_users_extensions" },
		{ "GET /helix/videos", "GET_helix_videos" },
		{ "GET /helix/webhooks/subscriptions", "GET_helix_webhooks_subscriptions" },
	};

	// Parse the requests into nodes.
	Parse(requests);

	// TODO:  print the Dispatcher class.
	Dump(root);
	Print(root);
}
