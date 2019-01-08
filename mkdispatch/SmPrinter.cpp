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

	private:
		NfaState::vector const& nfaStates;
		map machine;

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
	};

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

	// Print the function array.
	std::map<std::string, size_t> fnIndices;
	std::cout << "\tstatic void* fns[]= {" << std::endl;
	for(Request const& request : requests) {
		std::cout << "\t\t&" << request.fn << ", // " << fnIndices.size() << std::endl;
		fnIndices.insert({ request.fn, fnIndices.size() });
	}
	std::cout << "\t};" << std::endl;

	// Print the state machine.
	std::vector<size_t> finishingInnerParameterActions;
	std::vector<size_t> finishingFinalParameterActions;
	std::vector<size_t> finalStateActions;
	std::vector<size_t> startingParameterActions;
	std::vector<size_t> consumingParameterActions;
	std::vector<size_t> nextStateActions;
	size_t shift = machine.size() < 128 ? 0 : machine.size() < 32768 ? 8 : 24;
	size_t specialStateFlag = 0x80 << shift;
	auto const* type = machine.size() < 128 ? "char" : machine.size() < 32768 ? "short" : "int";
	std::cout << "\tstatic " << type << " states[" << machine.size() << "][256] = {" << std::endl;
	for(auto const& dfaState : machine) {
		std::cout << "\t\t{";
		std::vector<size_t> states(256, 0);
		for(auto const& pair : dfaState.second) {
			auto stateNumber = pair.second;
			size_t parameterNumber;
			if(pair.first == '\0') {
				// Find the index of the function of the NFA state.
				states[0] = fnIndices[nfaStates[pair.second].fn];
			} else if(pair.first == '\n') {
				states['\n'] = states['\r'] = states['?'] = specialStateFlag | finalStateActions.size();
				finishingInnerParameterActions.push_back(0);
				finishingFinalParameterActions.push_back(0); // TODO:  if this is finishing a parameter, set the correct value.
				finalStateActions.push_back(stateNumber);
				startingParameterActions.push_back(0);
				consumingParameterActions.push_back(0);
				nextStateActions.push_back(0);
			} else if(IsParameter(pair.first)) {
				for(char ch : chars) {
					states[ch] = stateNumber;
				}
			} else if(pair.first == '/' && (parameterNumber = machine.GetParameterNumber(pair.second), parameterNumber)) {
				states['/'] = specialStateFlag | finalStateActions.size();
				finishingInnerParameterActions.push_back(0); // TODO:  if this is finishing a parameter, set the correct value.
				finishingFinalParameterActions.push_back(0); // TODO:  if this is finishing a parameter, set the correct value.
				finalStateActions.push_back(0);
				startingParameterActions.push_back(parameterNumber);
				consumingParameterActions.push_back(0); // TODO
				nextStateActions.push_back(stateNumber);
			} else {
				states[pair.first] = stateNumber;
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
	PrintSpecialStateAction(finishingInnerParameterActions);
	PrintSpecialStateAction(finishingFinalParameterActions);
	PrintSpecialStateAction(finalStateActions);
	PrintSpecialStateAction(startingParameterActions);
	PrintSpecialStateAction(consumingParameterActions);
	PrintSpecialStateAction(nextStateActions);
}
