#include "pch.h"
#include "SmPrinter.h"

// This implementation does not handle variables in the path (e.g. :clientId).

namespace {
	std::string const chars = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz_-";

	struct NfaState {
		using pair = std::pair<char, size_t>;
		using vector = std::vector<NfaState>;

		std::vector<pair> transitions;
		std::string fn;

		static std::vector<NfaState> Create(Printer::vector const& requests) {
			std::vector<NfaState> rv;
			rv.emplace_back(NfaState{});
			size_t next = 0;
			for(Printer::Request const& request : requests) {
				size_t currentStateIndex = 0;
				for(char ch : request.line) {
					rv.emplace_back(NfaState{});
					rv[currentStateIndex].transitions.push_back({ ch, ++next });
					currentStateIndex = next;
					if(ch == ':') {
						rv[currentStateIndex].transitions.push_back({ ch, currentStateIndex });
					}
				}
				rv.emplace_back(NfaState{});
				rv[currentStateIndex].transitions.push_back({ '\n', ++next });
				rv[next].fn = request.fn;
			}
			return rv;
		}
	};

	struct DfaState {
		using map = std::map<size_t, std::map<char, size_t>>;

		static std::set<size_t> Move(NfaState::vector const& nfaStates, std::set<size_t> const& indices, char ch) {
			std::set<size_t> rv;
			for(size_t index : indices) {
				auto const& nfaState = nfaStates[index];
				for(NfaState::pair const& pair : nfaState.transitions) {
					if(pair.first == ch || (pair.first == ':' && chars.find(ch) != std::string::npos)) {
						rv.insert(pair.second);
					}
				}
			}
			return rv;
		}

		static map From(NfaState::vector const& nfaStates) {
			// p. 118
			std::map<std::set<size_t>, std::pair<bool, size_t>> states({ { { 0 }, { false, 0 } } });
			std::map<std::pair<size_t, char>, size_t> transitions;
			for(;;) {
				auto it = std::find_if(states.begin(), states.end(), [](auto const& pair) { return !pair.second.first; });
				if(it == states.end()) {
					break;
				}
				auto& pair = *it;
				auto const& t = pair.first;
				pair.second.first = true;
				auto symbols = CollectSymbols(t, nfaStates);
				for(char a : symbols) {
					auto u = Move(nfaStates, t, a);
					auto state = states.find(u);
					if(state == states.cend()) {
						state = states.insert({ u, { false, states.size() } }).first;
					}
					transitions.insert({ std::make_pair(pair.second.second, a), state->second.second });
				}
			}

			// Construct and return the state machine.
			//std::map<size_t, std::string> finalStates;
			//for(size_t i = 0, n = nfaStates.size(); i < n; ++i) {
			//	if(!nfaStates[i].fn.empty()) {
			//		finalStates.insert({ i, nfaStates[i].fn });
			//	}
			//}
			map machine;
			for(auto const& transition : transitions) {
				machine[transition.first.first][transition.first.second] = transition.second;
			}
			for(size_t i = 0, n = nfaStates.size(); i < n; ++i) {
				auto const& nfaState = nfaStates[i];
				if(!nfaState.fn.empty()) {
					// Find the corresponding DFA state number for this NFA state.
					auto it = std::find_if(states.cbegin(), states.cend(), [i](auto const& pair) {
						return pair.first.size() == 1 && pair.first.find(i) != pair.first.cend();
					});
					machine[it->second.second]['\0'] = i;
				}
			}

			return machine;
		}

		static std::set<char> CollectSymbols(std::set<size_t> const& nfaStateIndices, NfaState::vector const& nfaStates) {
			std::set<char> rv;
			for(size_t index : nfaStateIndices) {
				for(auto const& transition : nfaStates[index].transitions) {
					rv.insert(transition.first);
				}
			}
			return rv;
		}
	};
}

SmPrinter::SmPrinter() {}

SmPrinter::~SmPrinter() {}

void SmPrinter::InternalPrint(vector const& requests, Options const& options) {
	options;

	// Determine the maximum number of parameters in a request.
	ptrdiff_t nparameters = 0;
	for(Printer::Request const& request : requests) {
		nparameters = std::max(nparameters, std::count(request.line.cbegin(), request.line.cend(), ':'));
	}

	// Create a NFA of the requests.
	auto nfaStates = NfaState::Create(requests);

	// Convert the NFA into a DFA.
	auto machine = DfaState::From(nfaStates);

	// Print the function array.
	std::map<std::string, size_t> fnIndices;
	std::cout << "\tstatic void* fns[]= {" << std::endl;
	for(Request const& request : requests) {
		std::cout << "\t\t&" << request.fn << ", // " << fnIndices.size() << std::endl;
		fnIndices.insert({ request.fn, fnIndices.size() });
	}
	std::cout << "};" << std::endl;

	// Print the state machine.
	size_t finalStateFlag = machine.size() < 128 ? 0x80 : machine.size() < 32768 ? 0x8000 : 0x80000000;
	auto const* type = machine.size() < 128 ? "char" : machine.size() < 32768 ? "short" : "int";
	std::cout << "\tstatic " << type << " states[" << machine.size() << "][256] = {" << std::endl;
	for(auto const& dfaState : machine) {
		std::cout << "\t\t{";
		std::vector<size_t> states(256, 0);
		for(auto const& pair : dfaState.second) {
			auto stateNumber = pair.second;
			if(pair.first == '\0') {
				// Find the index of the function of the NFA state.
				auto fnIndex = fnIndices[nfaStates[pair.second].fn];
				states[0] = fnIndex;
			} else if(pair.first == '\n') {
				states['\n'] = finalStateFlag | stateNumber;
			} else if(pair.first == ':') {
				for(char ch : chars) {
					states[ch] = stateNumber;
				}
			} else {
				states[pair.first] = stateNumber;
			}
		}
		states.erase(std::find_if(states.crbegin(), states.crend(), [](auto state) { return state != 0; }).base(), states.cend());
		for(auto state : states) {
			if(state & finalStateFlag) {
				std::cout << "0x" << std::hex << finalStateFlag << std::dec << '|';
			}
			std::cout << (state & ~finalStateFlag) << ',';
		}
		std::cout << "}, // " << dfaState.first << std::endl;
	}
	std::cout << "\t};" << std::endl;
}
