#include "pch.h"
#include "mkapp.h"

namespace {
#undef tolower
	char tolower(char ch) {
		return static_cast<char>(std::tolower(static_cast<unsigned char>(ch)));
	}

#undef toupper
	char toupper(char ch) {
		return static_cast<char>(std::toupper(static_cast<unsigned char>(ch)));
	}

	std::string AsVariableName(std::string const& name) {
		auto i = name.find('-');
		std::string firstName = name.substr(0, i);
		auto& f = std::use_facet<std::ctype<char>>(std::locale());
		f.tolower(&firstName[0], &firstName[firstName.size()]);
		std::stringstream variableName;
		variableName << firstName;
		while(i != name.npos) {
			++i;
			auto j = name.find('-', i);
			auto nextName = name.substr(i, j - i);
			f.toupper(&nextName[0], &nextName[1]);
			f.tolower(&nextName[1], &nextName[nextName.size()]);
			variableName << nextName;
			i = j;
		}
		return variableName.str();
	}

	std::string AsManifestConstant(std::string const& name) {
		std::string manifestConstant;
		std::transform(name.cbegin(), name.cend(), std::back_inserter(manifestConstant), [](char ch) {
			return ch == '-' ? '_' : toupper(ch);
		});
		return manifestConstant;
	}
}

std::string mkheader(std::set<std::string, HeaderNameLess> const& names, std::ostream& out) {
	// Split the names into groups based on the first character.
	std::map<char, std::vector<std::remove_reference_t<decltype(names)>::value_type>> nameGroups;
	for(auto const& name : names) {
		nameGroups[name[0]].push_back(name);
	}

	// Print the class definition.
	std::string rv = ConstructHeaderClassName(names);
	out << "class " << rv << " : public HeaderBase {" << std::endl;
	out << "public:" << std::endl;
	out << '\t' << rv << "()";
	char separator = ':';
	for(auto const& name : names) {
		out << separator << ' ' << AsVariableName(name) << "()";
		separator = ',';
	}
	out << " {}" << std::endl;

	// Print the public property accessors.
	out << std::endl;
	for(auto const& name : names) {
		auto s = Capitalize(AsVariableName(name));
		out << "\t__declspec(property(get = Get" << s << ")) xstring const& " << s << ';' << std::endl;
	}

	// Print the public method accessors.
	out << std::endl;
	for(auto const& name : names) {
		auto s = Capitalize(AsVariableName(name));
		out << "\txstring const& Get" << s << "() const { return " << AsVariableName(name) << "; }" << std::endl;
	}

	// Print the CollectHeaderName method.
	out << std::endl;
	out << "\tbool CollectHeaderName(char const*& p, xstring*& q) override {" << std::endl;
	if(!nameGroups.empty()) {
		out << "\t\tswitch(*p) {" << std::endl;
		for(auto const& nameGroup : nameGroups) {
			out << "\t\tcase '" << tolower(nameGroup.first) << "':" << std::endl;
			out << "\t\tcase '" << toupper(nameGroup.first) << "':" << std::endl;
			for(auto const& name : nameGroup.second) {
				out << "\t\t\tif(_strnicmp(p + 1, \"" << name.substr(1) << ":\", " << name.size() << ") == 0) {" << std::endl;
				out << "\t\t\t\tp += " << (name.size() + 1) << ';' << std::endl;
				out << "\t\t\t\tq = &" << AsVariableName(name) << ';' << std::endl;
				out << "\t\t\t\treturn true;" << std::endl;
				out << "\t\t\t}" << std::endl;
			}
			out << "\t\t\tbreak;" << std::endl;
		}
		out << "\t\t}" << std::endl;
	}
	out << "\t\treturn __super::CollectHeaderName(p, q);" << std::endl;
	out << "\t}" << std::endl;

	// Print the member variables.
	out << std::endl;
	out << "private:" << std::endl;
	for(auto const& name : names) {
		out << "\txstring " << AsVariableName(name) << ';' << std::endl;
	}
	out << "};" << std::endl;

	return rv;
}
