#include "pch.h"
#include "SmPrinter.h"

// This implementation does not handle variables in the path (e.g. :clientId).

namespace {
	std::string const chars = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz_-";

	struct NfaState {
		using ptr = std::shared_ptr<NfaState>;
		using pair = std::pair<char, ptr>;
		using vector = std::vector<pair>;

		vector transitions;
		std::string fn;
		size_t number;
		static size_t next;

		NfaState(size_t number) : number(number) {}

		static ptr Create(Printer::vector const& requests) {
			ptr startState = std::make_shared<NfaState>(0);
			for(Printer::Request const& request : requests) {
				ptr currentState = startState;
				for(char ch : request.line) {
					currentState = currentState->PushState(ch);
					if(ch == ':') {
						currentState->transitions.push_back({ ch, currentState });
					}
				}
				currentState = currentState->PushState('\n');
				currentState->fn = request.fn;
			}
			return startState;
		}

		ptr PushState(char ch) {
			transitions.push_back({ ch, std::make_shared<NfaState>(++next) });
			auto nextState = transitions.back().second;
			return nextState;
		}
	};

	size_t NfaState::next;

	struct DfaState {
		using map = std::map<char, DfaState>;
		using vector = std::vector<NfaState::ptr>;

		vector nfaStates;
		map transitions;
		bool isMarked;
		__declspec(property(get = get_Name)) std::string Name;
		__declspec(property(get = get_Symbols)) std::set<char> Symbols;

		std::string get_Name() const {
			std::set<size_t> numbers;
			for(NfaState::ptr state : nfaStates) {
				numbers.insert(state->number);
			}
			std::ostringstream ss;
			for(size_t number : numbers) {
				ss << ',' << number;
			}
			return ss.str().substr(1);
		}

		std::set<char> get_Symbols() const {
			std::set<char> rv;
			for(NfaState::ptr state : nfaStates) {
				for(NfaState::pair const& pair : state->transitions) {
					rv.insert(pair.first);
				}
			}
			return rv;
		}

		DfaState Move(char ch) {
			DfaState rv{};
			for(NfaState::ptr state : nfaStates) {
				for(NfaState::pair const& pair : state->transitions) {
					if(pair.first == ch || (pair.first == ':' && ch != '\n')) {
						rv.nfaStates.push_back(pair.second);
					}
				}
			}
			return rv;
		}

		static std::map<std::string, DfaState> From(NfaState::ptr nfaStartState) {
			// p. 118
			std::map<std::string, DfaState> states({ { "0", DfaState{ { nfaStartState } } } });
			for(;;) {
				auto it = std::find_if(states.begin(), states.end(), [](decltype(states)::value_type& pair) { return !pair.second.isMarked; });
				if(it == states.cend()) {
					break;
				}
				auto& t = it->second;
				t.isMarked = true;
				for(char a : t.Symbols) {
					auto u = t.Move(a);
					auto state = states.find(u.Name);
					if(state == states.cend()) {
						states.insert({ u.Name, u });
						t.transitions.insert({ a, u });
					} else {
						t.transitions.insert({ a, state->second });
					}
				}
			}
			return states;
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
	NfaState::ptr nfaStartState = NfaState::Create(requests);

	// Convert the NFA into a DFA.
	auto dfaStates = DfaState::From(nfaStartState);

	// Map DFA state names to sequential numbers and vice versa.
	std::map<std::string, int> stateNumbers{ { "0", 0 } };
	std::vector<std::string> stateNames{ "0" };
	int next = 0;
	for(auto& pair : dfaStates) {
		if(stateNumbers.find(pair.first) == stateNumbers.cend()) {
			stateNumbers.insert({ pair.first, ++next });
			stateNames.push_back(pair.first);
		}
	}

	// Print the function array.
	std::map<std::string, size_t> fnIndices;
	std::cout << "\tstatic void* fns[]= {" << std::endl;
	for(Request const& request : requests) {
		std::cout << "\t\t&" << request.fn << ", // " << fnIndices.size() << std::endl;
		fnIndices.insert({ request.fn, fnIndices.size() });
	}
	std::cout << "};" << std::endl;

	// Print the state machine.
	auto const* type = stateNumbers.size() < 128 ? "char" : stateNumbers.size() < 32768 ? "short" : "int";
	std::cout << "\tstatic " << type << " states[" << stateNumbers.size() << "][256] = {" << std::endl;
	for(size_t i = 0, n = stateNumbers.size(); i < n; ++i) {
		std::cout << "\t\t{";
		auto const& dfaState = dfaStates[stateNames[i]];
		if(dfaState.transitions.empty()) {
			auto const& state = *dfaState.nfaStates.cbegin();
			std::cout << fnIndices[state->fn];
		} else {
			std::vector<int> states(256, 0);
			for(std::pair<char, DfaState> const& pair : dfaState.transitions) {
				int stateNumber = stateNumbers[pair.second.Name];
				if(pair.first == '\n') {
					states['\n'] = -stateNumber;
				} else if(pair.first == ':') {
					for(char ch : chars) {
						states[ch] = stateNumber;
					}
				} else {
					states[pair.first] = stateNumber;
				}
			}
			states.erase(std::find_if(states.crbegin(), states.crend(), [](int state) { return state != 0; }).base(), states.cend());
			for(int state : states) {
				std::cout << state << ',';
			}
		}
		std::cout << "}, // " << i << std::endl;
	}
	std::cout << "\t};" << std::endl;
}
