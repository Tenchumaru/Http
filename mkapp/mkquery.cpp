#include "pch.h"

void mkquery(std::map<std::string, bool> names, std::ostream* pout) {
	// Split the names into groups based on the first character.
	std::map<char, std::vector<std::remove_reference_t<decltype(names)>::value_type>> nameGroups;
	for(auto const& pair : names) {
		nameGroups[pair.first[0]].push_back(pair);
	}

	// Print the variables.
	*pout << "using xstring = std::pair<char const*, char const*>;" << std::endl;
	for(auto const& pair : names) {
		if(pair.second) {
			*pout << "std::vector<xstring>";
		} else {
			*pout << "xstring";
		}
		*pout << " Q" << pair.first << ';' << std::endl;
	}

	// Print the Initialize function.
	*pout << std::endl;
	*pout << "inline void Initialize() {" << std::endl;
	for(auto const& pair : names) {
		*pout << "\tQ" << pair.first;
		if(pair.second) {
			*pout << ".clear()";
		} else {
			*pout << " = xstring{}";
		}
		*pout << ';' << std::endl;
	}
	*pout << '}' << std::endl;

	// Print the CollectName function.
	*pout << std::endl;
	*pout << "bool CollectName(char const*& p, xstring*& q) {" << std::endl;
	if(!nameGroups.empty()) {
		*pout << "\tswitch(*p) {" << std::endl;
		for(auto const& nameGroup : nameGroups) {
			*pout << "\tcase '" << nameGroup.first << "':" << std::endl;
			for(auto const& pair : nameGroup.second) {
				*pout << "\t\tif(memcmp(p + 1, \"" << pair.first.substr(1) << "=\", " << pair.first.size() << ") == 0) {" << std::endl;
				*pout << "\t\t\tp += " << (pair.first.size() + 1) << ';' << std::endl;
				if(pair.second) {
					*pout << "\t\t\tQ" << pair.first << ".emplace_back(xstring{});" << std::endl;
					*pout << "\t\t\tq = &Q" << pair.first << ".back();" << std::endl;
				} else {
					*pout << "\t\t\tq = &Q" << pair.first << ';' << std::endl;
				}
				*pout << "\t\t\treturn true;" << std::endl;
				*pout << "\t\t}" << std::endl;
			}
			*pout << "\t\tbreak;" << std::endl;
		}
		*pout << "\t}" << std::endl;
	}

	// Print the rest of the CollectName function followed by the CollectValue
	// and CollectQuery functions.
#include "mkquery.inl"
}
