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
			std::for_each(requests.cbegin(), requests.cend(), [startState](Printer::Request const& request) {
				ptr currentState = startState;
				std::for_each(request.line.cbegin(), request.line.cend(), [&currentState](char ch) {
					currentState = currentState->PushState(ch);
					if(ch == ':') {
						currentState->transitions.push_back({ ch, currentState });
					}
				});
				currentState = currentState->PushState('\n');
				currentState->fn = request.fn;
			});
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
		using map = std::map<std::string, std::map<char, DfaState>>;
		using vector = std::vector<NfaState::ptr>;

		vector nfaStates;
		bool isMarked;
		__declspec(property(get = get_Name)) std::string Name;
		__declspec(property(get = get_Symbols)) std::set<char> Symbols;

		std::string get_Name() const {
			std::set<size_t> numbers;
			std::for_each(nfaStates.cbegin(), nfaStates.cend(), [&numbers](NfaState::ptr state) {
				numbers.insert(state->number);
			});
			std::ostringstream ss;
			std::for_each(numbers.cbegin(), numbers.cend(), [&ss](size_t number) {
				ss << ',' << number;
			});
			return ss.str().substr(1);
		}

		std::set<char> get_Symbols() const {
			std::set<char> rv;
			std::for_each(nfaStates.cbegin(), nfaStates.cend(), [&rv](NfaState::ptr state) {
				std::for_each(state->transitions.cbegin(), state->transitions.cend(), [&rv](NfaState::pair const& pair) {
					rv.insert(pair.first);
				});
			});
			return rv;
		}

		DfaState Move(char ch) {
			DfaState rv{};
			std::for_each(nfaStates.cbegin(), nfaStates.cend(), [ch, &rv](NfaState::ptr state) {
				std::for_each(state->transitions.cbegin(), state->transitions.cend(), [ch, &rv](NfaState::pair const& pair) {
					if(pair.first == ch || (pair.first == ':' && ch != '\n')) {
						rv.nfaStates.push_back(pair.second);
					}
				});
			});
			return rv;
		}

		static map From(NfaState::ptr nfaStartState) {
			// p. 118
			std::map<std::string, DfaState> dstates({ { "0", DfaState{ { nfaStartState } } } });
			map dtran;
			for(;;) {
				auto it = std::find_if(dstates.begin(), dstates.end(), [](decltype(dstates)::value_type& pair) { return !pair.second.isMarked; });
				if(it == dstates.cend()) {
					break;
				}
				auto& t = it->second;
				t.isMarked = true;
				for(char a : t.Symbols) {
					auto u = t.Move(a);
					if(dstates.find(u.Name) == dstates.cend()) {
						dstates.insert({ u.Name, u });
					}
					dtran[t.Name].insert({ a, u });
				}
			}
			return dtran;
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
	DfaState::map transitions = DfaState::From(nfaStartState);

	// Map DFA state names to sequential numbers and vice versa.
	std::map<std::string, int> stateNumbers{ { "0", 0 } };
	std::vector<std::string> stateNames{ "0" };
	int next = 0;
	std::for_each(transitions.cbegin(), transitions.cend(), [&next, &stateNames, &stateNumbers](DfaState::map::value_type const& pair) {
		if(stateNumbers.find(pair.first) == stateNumbers.cend()) {
			stateNumbers.insert({ pair.first, ++next });
			stateNames.push_back(pair.first);
		}
		std::for_each(pair.second.cbegin(), pair.second.cend(), [&next, &stateNames, &stateNumbers](std::pair<char, DfaState> const& pair) {
			if(stateNumbers.find(pair.second.Name) == stateNumbers.cend()) {
				stateNumbers.insert({ pair.second.Name, ++next });
				stateNames.push_back(pair.second.Name);
			}
		});
	});

	// Print the function array.
	std::map<std::string, size_t> fnIndices;
	std::cout << "\tstatic void* fns[]= {" << std::endl;
	std::for_each(requests.cbegin(), requests.cend(), [&fnIndices](Request const& request) {
		std::cout << "\t\t&" << request.fn << ", // " << fnIndices.size() << std::endl;
		fnIndices.insert({ request.fn, fnIndices.size() });
	});
	std::cout << "};" << std::endl;

	// Print the state machine.
	std::set<size_t> wasPrinted;
	auto const* type = stateNames.size() < 128 ? "char" : stateNames.size() < 32768 ? "short" : "int";
	std::cout << "\tstatic " << type << " states[" << stateNumbers.size() << "][256] = {" << std::endl;
	for(size_t i = 0; i < stateNames.size(); ++i) {
		for(DfaState::map::value_type const& transition : transitions) {
			if(stateNumbers[transition.first] == i) {
				if(!wasPrinted.insert(i).second) {
					std::cerr << "unexpected state " << i << std::endl;
				}
				std::vector<int> states(256, 0);
				for(std::pair<char, DfaState> const& pair : transition.second) {
					if(pair.first == '\n') {
						states['\n'] = -stateNumbers[pair.second.Name];
					} else if(pair.first == ':') {
						for(char ch : chars) {
							states[ch] = stateNumbers[pair.second.Name];
						}
					} else {
						states[pair.first] = stateNumbers[pair.second.Name];
					}
				}
				states.erase(std::find_if(states.crbegin(), states.crend(), [](int state) { return state != 0; }).base(), states.cend());
				std::cout << "\t\t{";
				for(int state : states) {
					std::cout << state << ',';
				}
				std::cout << "}, // " << i << std::endl;
			}
		}
		if(wasPrinted.find(i) == wasPrinted.cend()) {
			for(DfaState::map::value_type const& transition : transitions) {
				for(std::pair<char, DfaState> const& pair : transition.second) {
					if(stateNumbers[pair.second.Name] == i) {
						if(!wasPrinted.insert(i).second) {
							std::cerr << "unexpected state " << i << std::endl;
						}
						std::cout << "\t\t{";
						auto const& state = *pair.second.nfaStates.cbegin();
						std::cout << fnIndices[state->fn];
						std::cout << "}, // " << i << std::endl;
					}
				}
			}
		}
	}
	std::cout << "\t};" << std::endl;
}
