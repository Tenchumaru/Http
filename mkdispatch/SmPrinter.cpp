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
				char currentParameterNumber = -0x80;
				for(char ch : request.line) {
					if(ch == '/') {
						++currentParameterNumber;
					} else if(ch == ':') {
						ch = currentParameterNumber;
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
				auto const& t = it->first;
				it->second.first = true;
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
					machine[it->second.second][a] = state->second.second;
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
						std::cerr << "Note:  ambiguity found between";
						for(auto nfaStateNumber : it->first) {
							std::cerr << ' ' << nfaStates[nfaStateNumber].fn;
						}
						std::cerr << std::endl << "resolving in favor of " << nfaState.fn << std::endl;
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
							rv.insert({ transition.second, next.first + 0x80 });
						} else if(!wantsConsuming && nexts->second.size() > 1) {
							rv.insert({ transition.second, next.first + 0x80 });
						}
					}
				}
			}
		}

		void CollectFinishing(std::map<size_t, size_t>& rv, bool wantsFinal, size_t state, map::mapped_type::value_type const& transition) {
			if(transition.first) {
				if(state == transition.second) {
					if(!IsParameter(transition.first)) {
						throw std::logic_error("unexpected cirular state");
					}
					auto const& nexts = machine.find(transition.second);
					for(auto const& next : nexts->second) {
						if(next.first == (wantsFinal ? '\n' : '/')) {
							rv.insert({ next.second, transition.first + 0x80 });
						}
					}
				} else {
					auto const& transitions = machine[state];
					auto parameter = std::find_if(transitions.cbegin(), transitions.cend(), [](auto const& pair) {
						return IsParameter(pair.first);
					});
					if(parameter != transitions.cend()) {
						auto it = std::find_if(transitions.cbegin(), transitions.cend(), [wantsFinal](auto const& pair) {
							return pair.first == (wantsFinal ? '\n' : '/');
						});
						if(it != transitions.cend()) {
							rv.insert({ it->second, parameter->first + 0x80 });
						}
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

	void PrintSpecialStateIndex_(std::vector<size_t> const& specialStateIndices, char const* name, std::ostream& out) {
		out << "\tstatic state_t " << name << '[' << specialStateIndices.size() << "] = {" << std::endl << "\t\t";
		for(size_t specialStateIndex : specialStateIndices) {
			out << specialStateIndex << ',';
		}
		out << std::endl << "\t};" << std::endl;
	}
#define PrintSpecialStateIndex(indices) PrintSpecialStateIndex_(indices, #indices, out)
}

SmPrinter::SmPrinter() {}

SmPrinter::~SmPrinter() {}

void SmPrinter::InternalPrint(vector const& requests, Options const& options, std::ostream& out) {
	// Determine the maximum index of a segment containing a parameter in a request.
	ptrdiff_t nparameters = 0;
	for(Printer::Request const& request : requests) {
		auto it = std::find(request.line.crbegin(), request.line.crend(), ':');
		nparameters = std::max(nparameters, std::count(it, request.line.crend(), '/'));
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

	// Collect the function indices.
	std::map<std::string, size_t> fnIndices;
	for(Request const& request : requests) {
		fnIndices.insert({ request.fn, fnIndices.size() });
	}

	// Print the state machine.
	std::vector<size_t> parameterFinishingIndices;
	std::vector<size_t> functionIndices;
	std::vector<size_t> parameterStartingIndices;
	std::vector<size_t> parameterConsumingIndices;
	std::vector<size_t> nextStateIndices;
	size_t shift = machine.size() < 128 ? 0 : machine.size() < 32768 ? 8 : 24;
	size_t specialStateFlag = 0x80 << shift;
	auto const* type = machine.size() < 128 ? "char" : machine.size() < 32768 ? "short" : "int";
	out << "\tusing state_t = " << type << ';' << std::endl;
	out << "\tconstexpr state_t specialStateFlag = -0x" << std::hex << specialStateFlag << std::dec << ';' << std::endl;
	out << "\tstatic state_t states[" << machine.size() << "][256] = {" << std::endl;
	for(auto const& dfaState : machine) {
		out << "\t\t{";
		std::vector<size_t> states(256, 0);
		for(auto const& pair : dfaState.second) {
			char ch = pair.first;
			auto stateNumber = pair.second;
			if(ch == '\0') {
				// Skip this.  It's handled in the next block.
			} else if(ch == '\n') {
				states['\n'] = states['\r'] = states[' '] = states['?'] = specialStateFlag | functionIndices.size();
				auto finalParameterFinishingState = finalParameterFinishingStates.find(stateNumber);
				auto finalParameterFinishingIndex = finalParameterFinishingState == finalParameterFinishingStates.cend() ?
					0 : finalParameterFinishingState->second;
				parameterFinishingIndices.push_back(finalParameterFinishingIndex);
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
					parameterFinishingIndices.push_back(innerParameterFinishingIndex);
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
				out << "specialStateFlag|";
			}
			out << (state & ~specialStateFlag) << ',';
		}
		out << "}, // " << dfaState.first << std::endl;
	}
	out << "\t};" << std::endl;

	// Print the special state actions.
	PrintSpecialStateIndex(parameterFinishingIndices);
	PrintSpecialStateIndex(functionIndices);
	PrintSpecialStateIndex(parameterStartingIndices);
	PrintSpecialStateIndex(parameterConsumingIndices);
	PrintSpecialStateIndex(nextStateIndices);

	// Print the parameters.
	out << "\tchar const* parametersBegin[" << (nparameters + 1) << "];" << std::endl;
	out << "\tchar const* parametersEnd[" << (nparameters + 1) << "];" << std::endl;

	// Collect the function invocations.
	std::stringstream functionInvocations;
	for(size_t i = 0, n = requests.size(); i < n; ++i) {
		auto const& request = requests[i];
		functionInvocations << "\t\t\tcase " << (i + 1) << ':' << std::endl;
		functionInvocations << "\t\t\t\treturn " << request.fn << '(';
		for(auto it = request.line.cbegin(); it = std::find(it, request.line.cend(), ':'), it != request.line.cend(); ++it) {
			auto parameterIndex = std::count(request.line.cbegin(), it, '/');
			if(options.wantsStrings) {
				functionInvocations << "xstring(parametersBegin[" << parameterIndex << "], parametersEnd[" << parameterIndex << "])";
			} else {
				functionInvocations << "parametersBegin[" << parameterIndex << "], parametersEnd[" << parameterIndex << ']';
			}
			functionInvocations << ", ";
		}
		functionInvocations << "response";
		functionInvocations << ");" << std::endl;
	}

	// Print the main state machine loop.
	std::ifstream file(__FILE__);
	std::stringstream ss;
	ss << file.rdbuf();
	std::string content = ss.str();
	content.erase(0, content.find("// <<<\n") + 7);
	content.erase(content.find("\t// >>>"));
	std::string s("\t\t\tcase 0: break; // |||\n");
	auto it = content.find(s);
	content.erase(it, s.size());
	content.insert(it, functionInvocations.str());
	out << content;
}

void MainLoop() {
	char const* parametersBegin[1];
	char const* parametersEnd[1];
	using state_t = short;
	constexpr state_t specialStateFlag = -0x8000;
	std::vector<state_t> parameterFinishingIndices;
	std::vector<state_t> functionIndices;
	std::vector<state_t> parameterStartingIndices;
	std::vector<state_t> parameterConsumingIndices;
	std::vector<state_t> nextStateIndices;
	state_t states[1][256] = {};
	char const* p = nullptr;
	auto CollectQueries = [](char const*) { return false; };
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
		auto parameterFinishingIndex = parameterFinishingIndices[index];
		if(parameterFinishingIndex > 0) {
			parametersEnd[parameterFinishingIndex] = p - 1;
		}
		auto parameterConsumingIndex = parameterConsumingIndices[index];
		if(parameterConsumingIndex > 0) {
			parametersBegin[parameterConsumingIndex] = p;
			while(++p, *p != '/' && *p != '\n' && *p != '\r' && *p != ' ' && *p != '?') {
				continue;
			}
		}
		auto functionIndex = functionIndices[index];
		if(functionIndex > 0) {
			if(!CollectQueries(p - 1)) {
				break;
			}
			switch(functionIndex) {
			case 0: break; // |||
			}
		}
		auto parameterStartingIndex = parameterStartingIndices[index];
		if(parameterStartingIndex > 0) {
			parametersBegin[parameterStartingIndex] = p;
		}
		state = nextStateIndices[index];
	}
	// >>>
}
