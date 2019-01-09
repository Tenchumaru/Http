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
		map::const_iterator begin() const { return machine.cbegin(); }
		map::const_iterator end() const { return machine.cend(); }

		size_t GetParameterNumber(size_t stateNumber) const {
			auto state = machine.find(stateNumber);
			auto parameter = std::find_if(state->second.cbegin(), state->second.cend(), [](auto const& pair) { return IsParameter(pair.first); });
			return parameter == state->second.cend() ? 0 : parameter->first + 0x81;
		}

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
						std::cout << "// " << state << " -> / -> " << transition.second << " -> p" <<
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
						std::cout << "// " << state << " -> p" << (transition.first + 0x81) << " -> " <<
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

	void PrintSpecialStateAction_(char const* type, std::vector<size_t> const& specialStateActions, char const* name) {
		std::cout << "\tstatic " << type << ' ' << name << '[' << specialStateActions.size() << "] = {" << std::endl << "\t\t";
		for(size_t specialStateAction : specialStateActions) {
			std::cout << specialStateAction << ',';
		}
		std::cout << std::endl << "\t};" << std::endl;
	}
#define PrintSpecialStateAction(actions) PrintSpecialStateAction_(type, actions, #actions)
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
	auto parameterInnerFinishingStates = machine.CollectFinishing(false);
	auto parameterFinalFinishingStates = machine.CollectFinishing(true);

	// Print the function array.
	std::map<std::string, size_t> fnIndices;
	std::cout << "\tstatic void* fns[]= {" << std::endl;
	for(Request const& request : requests) {
		std::cout << "\t\t&" << request.fn << ", // " << fnIndices.size() << std::endl;
		fnIndices.insert({ request.fn, fnIndices.size() });
	}
	std::cout << "\t};" << std::endl;

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
	std::cout << "\tstatic " << type << " states[" << machine.size() << "][256] = {" << std::endl;
	for(auto const& dfaState : machine) {
		std::cout << "\t\t{";
		std::vector<size_t> states(256, 0);
		for(auto const& pair : dfaState.second) {
			char ch = pair.first;
			auto stateNumber = pair.second;
			if(ch == '\0') {
				// Find the index of the function of the NFA state.
				states[0] = fnIndices[nfaStates[stateNumber].fn];
			} else if(ch == '\n') {
				states['\n'] = states['\r'] = states['?'] = specialStateFlag | finalStateActions.size();
				innerParameterFinishingActions.push_back(0);
				auto const& parameterFinalFinishingState = parameterFinalFinishingStates.find(stateNumber);
				auto finalParameterFinishingAction = parameterFinalFinishingState == parameterFinalFinishingStates.cend() ?
					0 : parameterFinalFinishingState->second;
				finalParameterFinishingActions.push_back(finalParameterFinishingAction);
				finalStateActions.push_back(stateNumber);
				parameterStartingActions.push_back(0);
				parameterConsumingActions.push_back(0);
				nextStateActions.push_back(0);
			} else if(IsParameter(ch)) {
				for(char any : chars) {
					states[any] = stateNumber;
				}
			} else if(ch == '/') {
				states['/'] = specialStateFlag | finalStateActions.size();
				auto parameterInnerFinishingState = parameterInnerFinishingStates.find(stateNumber);
				auto parameterConsumingState = parameterConsumingStates.find(stateNumber);
				auto parameterStartingState = parameterStartingStates.find(stateNumber);
				bool isNormal = parameterInnerFinishingState == parameterInnerFinishingStates.cend() &&
					parameterConsumingState == parameterConsumingStates.cend() &&
					parameterStartingState == parameterStartingStates.cend();
				if(isNormal) {
					states[ch] = stateNumber;
				} else {
					auto innerParameterFinishingAction = parameterInnerFinishingState == parameterInnerFinishingStates.cend() ?
						0 : parameterInnerFinishingState->second;
					innerParameterFinishingActions.push_back(innerParameterFinishingAction);
					finalParameterFinishingActions.push_back(0);
					finalStateActions.push_back(0);
					auto parameterStartingAction = parameterStartingState == parameterStartingStates.cend() ? 0 : parameterStartingState->second;
					parameterStartingActions.push_back(parameterStartingAction);
					auto parameterConsumingAction = parameterConsumingState == parameterConsumingStates.cend() ? 0 : parameterConsumingState->second;
					parameterConsumingActions.push_back(parameterConsumingAction);
					nextStateActions.push_back(stateNumber);
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
}
