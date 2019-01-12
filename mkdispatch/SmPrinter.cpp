#include "pch.h"
#include "SmPrinter.h"

namespace {
	std::string const chars = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz_-";

	bool IsParameter(char ch) {
		return ch < 0;
	}

	struct NfaState {
		using vector = std::vector<NfaState>;

		std::vector<std::pair<char, size_t>> transitions;
		std::string fn;

		static std::vector<NfaState> Create(Printer::vector const& requests) {
			std::vector<NfaState> rv;
			rv.emplace_back(NfaState{});
			size_t next = 0;
			for(Printer::Request const& request : requests) {
				size_t currentStateIndex = 0;
				char currentParameterNumber = 0x7f;
				for(char ch : request.line) {
					if(ch == ':') {
						ch = ++currentParameterNumber;
					}
					rv.emplace_back(NfaState{});
					rv[currentStateIndex].transitions.push_back({ ch, ++next });
					currentStateIndex = next;
					if(ch == currentParameterNumber) {
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

	class StateMachine {
	public:
		using map = std::map<size_t, std::map<char, size_t>>;

		StateMachine(NfaState::vector const& nfaStates) : nfaStates(nfaStates) {
			// p. 118
			std::map<std::set<size_t>, std::pair<bool, size_t>> states({ { { 0 }, { false, 0 } } });
			for(;;) {
				auto it = std::find_if(states.begin(), states.end(), [](auto const& pair) { return !pair.second.first; });
				if(it == states.end()) {
					break;
				}
				auto& pair = *it;
				auto const& t = pair.first;
				pair.second.first = true;
				auto symbols = CollectSymbols(t);
				for(char a : symbols) {
					auto u = Move(t, a);
					auto state = states.find(u);
					if(state == states.cend()) {
						auto p = states.insert({ u, { false, states.size() } });
						if(!p.second) {
							throw std::logic_error("failed state insertion");
						}
						state = p.first;
					}
					machine[pair.second.second][a] = state->second.second;
				}
			}

			// Set the final states.
			for(size_t i = 0, n = nfaStates.size(); i < n; ++i) {
				auto const& nfaState = nfaStates[i];
				if(!nfaState.fn.empty()) {
					// Find the corresponding DFA state number for this NFA state.
					auto it = std::find_if(states.cbegin(), states.cend(), [i](auto const& pair) {
						return pair.first.size() == 1 && pair.first.find(i) != pair.first.cend();
					});
					if(it == states.cend()) {
						// It's possible the grammar is ambiguous but can be
						// resolved by giving non-any priority over any.
						it = std::find_if(states.cbegin(), states.cend(), [i](auto const& pair) {
							return pair.first.find(i) != pair.first.cend();
						});
					}
					machine[it->second.second]['\0'] = i;
				}
			}

			// Check for an ambiguous grammar.
			if(states.size() != machine.size()) {
				std::cerr << "error:  ambiguous grammar: ";
				auto it = std::find_if(states.cbegin(), states.cend(), [this](auto const& pair) { return machine.find(pair.second.second) == machine.cend(); });
				for(auto nfaStateNumber : it->first) {
					auto const& nfaState = nfaStates[nfaStateNumber];
					std::cerr << ' ' << nfaState.fn;
				}
				std::cerr << std::endl;
				exit(1);
			}
		}

		size_t size() const { return machine.size(); }
		map::mapped_type const& operator[](size_t index) const { return machine.find(index)->second; }
		map::const_iterator begin() const { return machine.cbegin(); }
		map::const_iterator end() const { return machine.cend(); }

		std::map<size_t, size_t> CollectStarting(bool wantsConsuming) {
			auto fn = [this, wantsConsuming](std::map<size_t, size_t>& rv, size_t /*state*/, map::mapped_type::value_type const& transition) {
				CollectStarting(rv, wantsConsuming, transition);
			};
			return Collect(fn);
		}

		std::map<size_t, size_t> CollectFinishing(bool wantsFinal) {
			auto fn = [this, wantsFinal](std::map<size_t, size_t>& rv, size_t state, map::mapped_type::value_type const& transition) {
				CollectFinishing(rv, wantsFinal, state, transition);
			};
			return Collect(fn);
		}

	private:
		NfaState::vector const& nfaStates;
		map machine;

		void CollectStarting(std::map<size_t, size_t>& rv, bool wantsConsuming, map::mapped_type::value_type const& transition) {
			if(transition.first == '/') {
				auto const& nexts = machine.find(transition.second);
				for(auto const& next : nexts->second) {
					if(IsParameter(next.first)) {
						if(wantsConsuming && nexts->second.size() == 1) {
							rv.insert({ transition.second, next.first + 0x81 });
						} else if(!wantsConsuming && nexts->second.size() > 1) {
							rv.insert({ transition.second, next.first + 0x81 });
						}
					}
				}
			}
		}

		void CollectFinishing(std::map<size_t, size_t>& rv, bool wantsFinal, size_t state, map::mapped_type::value_type const& transition) {
			if(state == transition.second) {
				if(!IsParameter(transition.first)) {
					std::cerr << "warning: unexpected cirular state on '" << transition.first << '\'' << std::endl;
				}
				auto const& nexts = machine.find(transition.second);
				for(auto const& next : nexts->second) {
					if(next.first == (wantsFinal ? '\n' : '/')) {
						rv.insert({ next.second, transition.first + 0x81 });
					}
				}
			}
		}

		std::map<size_t, size_t> Collect(std::function<void(std::map<size_t, size_t>&, size_t, map::mapped_type::value_type const&)> fn) {
			std::map<size_t, size_t> rv;
			std::set<size_t> visited;
			std::vector<size_t> toVisit;
			toVisit.push_back(0);
			while(!toVisit.empty()) {
				size_t state = toVisit[0];
				toVisit.erase(toVisit.cbegin());
				visited.insert(state);
				auto const& transitions = machine.find(state);
				for(auto const& transition : transitions->second) {
					fn(rv, state, transition);
					if(transition.first && visited.find(transition.second) == visited.cend()) {
						toVisit.push_back(transition.second);
					}
				}
			}
			return rv;
		}

		std::set<size_t> Move(std::set<size_t> const& indices, char ch) {
			std::set<size_t> rv;
			for(size_t index : indices) {
				auto const& nfaState = nfaStates[index];
				for(auto const& pair : nfaState.transitions) {
					if(pair.first == ch || (IsParameter(pair.first) && chars.find(ch) != std::string::npos)) {
						rv.insert(pair.second);
					}
				}
			}
			return rv;
		}

		std::set<char> CollectSymbols(std::set<size_t> const& nfaStateIndices) {
			std::set<char> rv;
			for(size_t index : nfaStateIndices) {
				for(auto const& transition : nfaStates[index].transitions) {
					rv.insert(transition.first);
				}
			}
			return rv;
		}
	};

	void PrintSpecialStateIndex_(std::vector<size_t> const& specialStateIndices, char const* name) {
		std::cout << "\tstatic state_t " << name << '[' << specialStateIndices.size() << "] = {" << std::endl << "\t\t";
		for(size_t specialStateIndex : specialStateIndices) {
			std::cout << specialStateIndex << ',';
		}
		std::cout << std::endl << "\t};" << std::endl;
	}
#define PrintSpecialStateIndex(actions) PrintSpecialStateIndex_(actions, #actions)
}

SmPrinter::SmPrinter() {}

SmPrinter::~SmPrinter() {}

void SmPrinter::InternalPrint(vector const& requests, Options const& options) {
	// Determine the maximum number of parameters in a request.
	ptrdiff_t nparameters = 0;
	for(Printer::Request const& request : requests) {
		nparameters = std::max(nparameters, std::count(request.line.cbegin(), request.line.cend(), ':'));
	}

	// Create a NFA of the requests.
	auto nfaStates = NfaState::Create(requests);

	// Convert the NFA into a DFA.
	StateMachine machine(nfaStates);

	// Collect the parameter tracking states.
	auto parameterStartingStates = machine.CollectStarting(false);
	auto parameterConsumingStates = machine.CollectStarting(true);
	auto innerParameterFinishingStates = machine.CollectFinishing(false);
	auto finalParameterFinishingStates = machine.CollectFinishing(true);

	// Print the parameters.
	std::cout << "\tchar const* begin[" << (nparameters + 1) << "];" << std::endl;
	std::cout << "\tchar const* end[" << (nparameters + 1) << "];" << std::endl;

	// Collect the function indices.
	std::map<std::string, size_t> fnIndices;
	for(Request const& request : requests) {
		fnIndices.insert({ request.fn, fnIndices.size() });
	}

	// Print the state machine.
	std::vector<size_t> innerParameterFinishingIndices;
	std::vector<size_t> finalParameterFinishingIndices;
	std::vector<size_t> functionIndices;
	std::vector<size_t> parameterStartingIndices;
	std::vector<size_t> parameterConsumingIndices;
	std::vector<size_t> nextStateIndices;
	size_t shift = machine.size() < 128 ? 0 : machine.size() < 32768 ? 8 : 24;
	size_t specialStateFlag = 0x80 << shift;
	auto const* type = machine.size() < 128 ? "char" : machine.size() < 32768 ? "short" : "int";
	std::cout << "\tusing state_t = " << type << ';' << std::endl;
	std::cout << "\tstate_t constexpr specialStateFlag = -0x" << std::hex << specialStateFlag << std::dec << ';' << std::endl;
	std::cout << "\tstatic state_t states[" << machine.size() << "][256] = {" << std::endl;
	for(auto const& dfaState : machine) {
		std::cout << "\t\t{";
		std::vector<size_t> states(256, 0);
		for(auto const& pair : dfaState.second) {
			char ch = pair.first;
			auto stateNumber = pair.second;
			if(ch == '\0') {
				// Skip this.  It's handled in the next block.
			} else if(ch == '\n') {
				states['\n'] = states['\r'] = states['?'] = specialStateFlag | functionIndices.size();
				innerParameterFinishingIndices.push_back(0);
				auto const& finalParameterFinishingState = finalParameterFinishingStates.find(stateNumber);
				auto finalParameterFinishingIndex = finalParameterFinishingState == finalParameterFinishingStates.cend() ?
					0 : finalParameterFinishingState->second;
				finalParameterFinishingIndices.push_back(finalParameterFinishingIndex);
				functionIndices.push_back(fnIndices[nfaStates[machine[stateNumber].cbegin()->second].fn] + 1);
				parameterStartingIndices.push_back(0);
				parameterConsumingIndices.push_back(0);
				nextStateIndices.push_back(0);
			} else if(IsParameter(ch)) {
				for(char any : chars) {
					states[any] = stateNumber;
				}
			} else if(ch == '/') {
				auto innerParameterFinishingState = innerParameterFinishingStates.find(stateNumber);
				auto parameterConsumingState = parameterConsumingStates.find(stateNumber);
				auto parameterStartingState = parameterStartingStates.find(stateNumber);
				bool isNormal = innerParameterFinishingState == innerParameterFinishingStates.cend() &&
					parameterConsumingState == parameterConsumingStates.cend() &&
					parameterStartingState == parameterStartingStates.cend();
				if(isNormal) {
					states['/'] = stateNumber;
				} else {
					states['/'] = specialStateFlag | functionIndices.size();
					auto innerParameterFinishingIndex = innerParameterFinishingState == innerParameterFinishingStates.cend() ?
						0 : innerParameterFinishingState->second;
					innerParameterFinishingIndices.push_back(innerParameterFinishingIndex);
					finalParameterFinishingIndices.push_back(0);
					functionIndices.push_back(0);
					auto parameterConsumingIndex = parameterConsumingState == parameterConsumingStates.cend() ? 0 : parameterConsumingState->second;
					if(parameterConsumingIndex) {
						parameterConsumingIndices.push_back(parameterConsumingIndex);
						stateNumber = machine[stateNumber].cbegin()->second;
						parameterStartingIndices.push_back(0);
						nextStateIndices.push_back(stateNumber);
					} else {
						auto parameterStartingIndex = parameterStartingState == parameterStartingStates.cend() ? 0 : parameterStartingState->second;
						parameterStartingIndices.push_back(parameterStartingIndex);
						parameterConsumingIndices.push_back(0);
						nextStateIndices.push_back(stateNumber);
					}
				}
			} else {
				states[ch] = stateNumber;
			}
		}
		states.erase(std::find_if(states.crbegin(), states.crend(), [](auto state) { return state != 0; }).base(), states.cend());
		for(auto state : states) {
			if(state & specialStateFlag) {
				std::cout << "specialStateFlag|";
			}
			std::cout << (state & ~specialStateFlag) << ',';
		}
		std::cout << "}, // " << dfaState.first << std::endl;
	}
	std::cout << "\t};" << std::endl;

	// Print the special state actions.
	PrintSpecialStateIndex(innerParameterFinishingIndices);
	PrintSpecialStateIndex(finalParameterFinishingIndices);
	PrintSpecialStateIndex(functionIndices);
	PrintSpecialStateIndex(parameterStartingIndices);
	PrintSpecialStateIndex(parameterConsumingIndices);
	PrintSpecialStateIndex(nextStateIndices);

	// Collect the function invocations.
	std::stringstream functionInvocations;
	for(size_t i = 0, n = requests.size(); i < n; ++i) {
		auto const& request = requests[i];
		functionInvocations << "\t\t\tcase " << (i + 1) << ':' << std::endl;
		functionInvocations << "\t\t\t\treturn " << request.fn << '(';
		for(size_t j = 1, m = std::count(request.line.cbegin(), request.line.cend(), ':'); j <= m; ++j) {
			if(options.wantsStrings) {
				functionInvocations << "xstring(begin[" << j << "], end[" << j << "])";
			} else {
				functionInvocations << "begin[" << j << "], end[" << j << ']';
			}
			if(j < m) {
				functionInvocations << ", ";
			}
		}
		functionInvocations << ");" << std::endl;
	}

	// Print the main state machine loop.
	std::ifstream file(__FILE__);
	std::stringstream ss;
	ss << file.rdbuf();
	std::string content = ss.str();
	content.erase(0, content.find("// <<<\n") + 7);
	content.erase(content.find("\t// >>>"));
	auto it = content.find("\t\t\tcase 0: break; // |||\n");
	content.erase(it, 25);
	content.insert(it, functionInvocations.str());
	std::cout << content;
}

void MainLoop() {
	char const* begin[1];
	char const* end[1];
	using state_t = short;
	state_t constexpr specialStateFlag = -0x8000;
	std::vector<state_t> innerParameterFinishingIndices;
	std::vector<state_t> finalParameterFinishingIndices;
	std::vector<state_t> functionIndices;
	std::vector<state_t> parameterStartingIndices;
	std::vector<state_t> parameterConsumingIndices;
	std::vector<state_t> nextStateIndices;
	state_t states[1][256] = {};
	char const* p = nullptr;
	// <<<
	for(state_t state = 0;;) {
		do {
			char ch = *p++;
			state = states[state][ch];
		} while(state > 0);
		if(state == 0) {
			break;
		}
		int index = state & ~specialStateFlag;
		auto innerParameterFinishingIndex = innerParameterFinishingIndices[index];
		if(innerParameterFinishingIndex > 0) {
			end[innerParameterFinishingIndex] = p - 1;
		}
		auto finalParameterFinishingIndex = finalParameterFinishingIndices[index];
		if(finalParameterFinishingIndex > 0) {
			end[finalParameterFinishingIndex] = p - 1;
		}
		auto parameterConsumingIndex = parameterConsumingIndices[index];
		if(parameterConsumingIndex > 0) {
			begin[parameterConsumingIndex] = p;
			while(++p, *p != '/' && *p != '\n' && *p != '\r' && *p != '?') {
				continue;
			}
		}
		auto functionIndex = functionIndices[index];
		if(functionIndex > 0) {
			switch(functionIndex) {
			case 0: break; // |||
			}
		}
		auto parameterStartingIndex = parameterStartingIndices[index];
		if(parameterStartingIndex > 0) {
			begin[parameterStartingIndex] = p;
		}
		state = nextStateIndices[index];
	}
	// >>>
}
