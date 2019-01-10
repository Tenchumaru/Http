#include "pch.h"
#include "SmPrinter.h"

// This implementation does not handle variables in the path (e.g. :clientId).

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
						state = states.insert({ u, { false, states.size() } }).first;
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
					machine[it->second.second]['\0'] = i;
				}
			}
		}

		size_t size() const { return machine.size(); }
		map::mapped_type const& operator[](size_t index) const { return machine.find(index)->second; }
		map::const_iterator begin() const { return machine.cbegin(); }
		map::const_iterator end() const { return machine.cend(); }

		std::map<size_t, size_t> CollectStarting(bool wantsConsuming) {
			auto fn = [this, wantsConsuming](std::map<size_t, size_t>& rv, size_t state, map::mapped_type::value_type const& transition) {
				CollectStarting(rv, wantsConsuming, state, transition);
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

		void CollectStarting(std::map<size_t, size_t>& rv, bool wantsConsuming, size_t state, map::mapped_type::value_type const& transition) {
			if(transition.first == '/') {
				auto const& nexts = machine.find(transition.second);
				for(auto const& next : nexts->second) {
					if(IsParameter(next.first)) {
						std::cout << "\t// " << state << " -> / -> " << transition.second << " -> p" <<
							(next.first + 0x81) << " -> " << next.second << " (" << nexts->second.size() <<
							" transitions)" << std::endl;
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
						std::cout << "\t// " << state << " -> p" << (transition.first + 0x81) << " -> " <<
							transition.second << " -> " << (wantsFinal ? '$' : '/') << " -> " <<
							next.second << " (" << nexts->second.size() << " transitions)" << std::endl;
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
					if(pair.first == ch || (pair.first == ':' && chars.find(ch) != std::string::npos)) {
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

	void PrintSpecialStateAction_(std::vector<size_t> const& specialStateActions, char const* name) {
		std::cout << "\tstatic state_t " << name << '[' << specialStateActions.size() << "] = {" << std::endl << "\t\t";
		for(size_t specialStateAction : specialStateActions) {
			std::cout << specialStateAction << ',';
		}
		std::cout << std::endl << "\t};" << std::endl;
	}
#define PrintSpecialStateAction(actions) PrintSpecialStateAction_(actions, #actions)
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
	StateMachine machine(nfaStates);

	// Collect the parameter tracking states.
	auto parameterStartingStates = machine.CollectStarting(false);
	auto parameterConsumingStates = machine.CollectStarting(true);
	auto innerParameterFinishingStates = machine.CollectFinishing(false);
	auto finalParameterFinishingStates = machine.CollectFinishing(true);

	// Print the function array.
	std::map<std::string, size_t> fnIndices;
	std::cout << "\tstatic std::function<void()> fns[] = {" << std::endl;
	for(Request const& request : requests) {
		std::cout << "\t\t[] { " << request.fn << '(';
		for(size_t i = 0, n = std::count(request.line.cbegin(), request.line.cend(), ':'); i < n; ++i) {
			std::cout << "begin[" << i << "], end[" << i << ']';
			if(i + 1 < n) {
				std::cout << ", ";
			}
		}
		std::cout << "); }, // " << fnIndices.size() << std::endl;
		fnIndices.insert({ request.fn, fnIndices.size() });
	}
	std::cout << "\t};" << std::endl;

	// Print the parameters.
	std::cout << "\tchar const* begin[" << (nparameters + 1) << "];" << std::endl;
	std::cout << "\tchar const* end[" << (nparameters + 1) << "];" << std::endl;

	// Print the state machine.
	std::vector<size_t> innerParameterFinishingActions;
	std::vector<size_t> finalParameterFinishingActions;
	std::vector<size_t> finalStateActions;
	std::vector<size_t> parameterStartingActions;
	std::vector<size_t> parameterConsumingActions;
	std::vector<size_t> nextStateActions;
	size_t shift = machine.size() < 128 ? 0 : machine.size() < 32768 ? 8 : 24;
	size_t specialStateFlag = 0x80 << shift;
	auto const* type = machine.size() < 128 ? "char" : machine.size() < 32768 ? "short" : "int";
	std::cout << "\tusing state_t = " << type << ';' << std::endl;
	std::cout << "\tstatic state_t states[" << machine.size() << "][256] = {" << std::endl;
	for(auto const& dfaState : machine) {
		std::cout << "\t\t{";
		std::vector<size_t> states(256, 0);
		for(auto const& pair : dfaState.second) {
			char ch = pair.first;
			auto stateNumber = pair.second;
			if(ch == '\0') {
				// Find the index of the function of the NFA state.
				states[0] = fnIndices[nfaStates[stateNumber].fn]; // TODO:  remove this.  See next TODO comment.
			} else if(ch == '\n') {
				states['\n'] = states['\r'] = states['?'] = specialStateFlag | finalStateActions.size();
				innerParameterFinishingActions.push_back(0);
				auto const& finalParameterFinishingState = finalParameterFinishingStates.find(stateNumber);
				auto finalParameterFinishingAction = finalParameterFinishingState == finalParameterFinishingStates.cend() ?
					0 : finalParameterFinishingState->second;
				finalParameterFinishingActions.push_back(finalParameterFinishingAction);
				finalStateActions.push_back(stateNumber); // TODO:  look up the function index instead of the state containing the function index.
				parameterStartingActions.push_back(0);
				parameterConsumingActions.push_back(0);
				nextStateActions.push_back(0);
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
					states['/'] = specialStateFlag | finalStateActions.size();
					auto innerParameterFinishingAction = innerParameterFinishingState == innerParameterFinishingStates.cend() ?
						0 : innerParameterFinishingState->second;
					innerParameterFinishingActions.push_back(innerParameterFinishingAction);
					finalParameterFinishingActions.push_back(0);
					finalStateActions.push_back(0);
					auto parameterConsumingAction = parameterConsumingState == parameterConsumingStates.cend() ? 0 : parameterConsumingState->second;
					if(parameterConsumingAction) {
						parameterConsumingActions.push_back(parameterConsumingAction);
						stateNumber = machine[stateNumber].cbegin()->second;
						parameterStartingActions.push_back(0);
						nextStateActions.push_back(stateNumber);
					} else {
						auto parameterStartingAction = parameterStartingState == parameterStartingStates.cend() ? 0 : parameterStartingState->second;
						parameterStartingActions.push_back(parameterStartingAction);
						parameterConsumingActions.push_back(0);
						nextStateActions.push_back(stateNumber);
					}
				}
			} else {
				states[ch] = stateNumber;
			}
		}
		states.erase(std::find_if(states.crbegin(), states.crend(), [](auto state) { return state != 0; }).base(), states.cend());
		for(auto state : states) {
			if(state & specialStateFlag) {
				std::cout << "0x" << std::hex << specialStateFlag << std::dec << '|';
			}
			std::cout << (state & ~specialStateFlag) << ',';
		}
		std::cout << "}, // " << dfaState.first << std::endl;
	}
	std::cout << "\t};" << std::endl;

	// Print the special state actions.
	PrintSpecialStateAction(innerParameterFinishingActions);
	PrintSpecialStateAction(finalParameterFinishingActions);
	PrintSpecialStateAction(finalStateActions);
	PrintSpecialStateAction(parameterStartingActions);
	PrintSpecialStateAction(parameterConsumingActions);
	PrintSpecialStateAction(nextStateActions);

	// Print the main state machine loop.
	std::ifstream file(__FILE__);
	std::stringstream ss;
	ss << file.rdbuf();
	std::string content = ss.str();
	content.erase(0, content.find("// <<<\n") + 7);
	content.erase(content.find("\t// >>>"));
	std::cout << content;
}

void MainLoop() {
	static std::function<void()> fns[] = {
		[] { fns[0](); },
	};
	char const* begin[1];
	char const* end[1];
	using state_t = short;
	std::vector<state_t> innerParameterFinishingActions;
	std::vector<state_t> finalParameterFinishingActions;
	std::vector<state_t> finalStateActions;
	std::vector<state_t> parameterStartingActions;
	std::vector<state_t> parameterConsumingActions;
	std::vector<state_t> nextStateActions;
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
		int index = state & ~0x80;
		auto innerParameterFinishingAction = innerParameterFinishingActions[index];
		if(innerParameterFinishingAction > 0) {
			end[innerParameterFinishingAction] = p - 1;
		}
		auto finalParameterFinishingAction = finalParameterFinishingActions[index];
		if(finalParameterFinishingAction > 0) {
			end[finalParameterFinishingAction] = p - 1;
		}
		auto finalStateAction = finalStateActions[index];
		if(finalStateAction > 0) {
			auto fn = fns[states[finalStateAction][0]];
			fn();
		}
		auto parameterStartingAction = parameterStartingActions[index];
		if(parameterStartingAction > 0) {
			begin[parameterStartingAction] = p;
		}
		auto parameterConsumingAction = parameterConsumingActions[index];
		if(parameterConsumingAction > 0) {
			begin[parameterStartingAction] = p;
			while(++p, *p != '/' && *p != '\n' && *p != '\r' && *p != '?') { // TODO:  separate this into inner and final.
				continue;
			}
			// TODO:  consider collapsing the corresponding finishing action into this one.  Then, have it go to the state after
			// reading the delimiter.  In the case of an inner parameter, it's a regular state.  In the case of a final parameter,
			// it's a final state.
		}
		state = nextStateActions[index];
	}
	// >>>
}
