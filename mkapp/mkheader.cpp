#include "pch.h"

namespace {
#undef tolower
	char tolower(char ch) {
		return static_cast<char>(std::tolower(static_cast<unsigned char>(ch)));
	}

#undef toupper
	char toupper(char ch) {
		return static_cast<char>(std::toupper(static_cast<unsigned char>(ch)));
	}
}

void mkheader(std::vector<std::pair<std::string, std::string>> const& names, std::ostream* pout) {
	// Split the names into groups based on the first character.
	std::map<char, std::vector<std::remove_reference_t<decltype(names)>::value_type>> nameGroups;
	for(auto const& pair : names) {
		nameGroups[pair.first[0]].push_back(pair);
	}

	// Print the variables.
	*pout << "using xstring = std::pair<char const*, char const*>;" << std::endl;
	for(auto const& pair : names) {
		*pout << "xstring H" << pair.second << ';' << std::endl;
	}

	// Print the Initialize function.
	*pout << std::endl;
	*pout << "inline void Initialize() {" << std::endl;
	for(auto const& pair : names) {
		*pout << "\tH" << pair.second << " = xstring{};" << std::endl;
	}
	*pout << '}' << std::endl;

	// Print the CollectName function.
	*pout << std::endl;
	*pout << "bool CollectName(char const*& p, xstring*& q) {" << std::endl;
	if(!nameGroups.empty()) {
		*pout << "\tswitch(*p) {" << std::endl;
		for(auto const& nameGroup : nameGroups) {
			*pout << "\tcase '" << tolower(nameGroup.first) << "':" << std::endl;
			*pout << "\tcase '" << toupper(nameGroup.first) << "':" << std::endl;
			for(auto const& pair : nameGroup.second) {
				*pout << "\t\tif(_strnicmp(p + 1, \"" << pair.first.substr(1) << ":\", " << pair.first.size() << ") == 0) {" << std::endl;
				*pout << "\t\t\tp += " << (pair.first.size() + 1) << ';' << std::endl;
				*pout << "\t\t\tq = &H" << pair.second << ';' << std::endl;
				*pout << "\t\t\treturn true;" << std::endl;
				*pout << "\t\t}" << std::endl;
			}
			*pout << "\t\tbreak;" << std::endl;
		}
		*pout << "\t}" << std::endl;
	}

	// Print the rest of the CollectName function followed by the CollectValue
	// and CollectHeaders functions.
#include "mkheader.inl"
}
