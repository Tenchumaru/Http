#include "pch.h"
#include "mkapp.h"

std::string mkquery(std::set<std::string> const& names, std::ostream& out) {
	std::map<std::string, int> pairs;
	std::transform(names.cbegin(), names.cend(), std::inserter(pairs, pairs.end()), [](std::string const& name) {
		auto i = name.find('[');
		if(i != name.npos) {
			return std::make_pair(name.substr(0, i), std::stoi(name.substr(i + 1)));
		} else {
			return std::make_pair(name, 0);
		}
	});

	// Split the names into groups based on the first character.
	std::map<char, std::vector<decltype(pairs)::value_type>> nameGroups;
	std::string rv = "Query";
	for(auto const& pair : pairs) {
		auto const& name = pair.first;
		nameGroups[name[0]].push_back(pair);
		rv += '_';
		rv += name;
		if(pair.second) {
			rv += '_';
		}
	}

	// Print the class definition.
	out << "class " << rv << " : public QueryBase {" << std::endl;
	out << "public:" << std::endl;
	out << '\t' << rv << "()";
	char separator = ':';
	for(auto const& pair : pairs) {
		if(!pair.second) {
			out << separator << ' ' << pair.first << "()";
			separator = ',';
		}
	}
	out << " {}" << std::endl;

	// Print the public property accessors.
	out << std::endl;
	for(auto const& pair : pairs) {
		auto s = Capitalize(pair.first);
		auto type = GetType(pair.second);
		out << "\t__declspec(property(get = Get" << s << ")) " << type << " const& " << s << ';' << std::endl;
	}

	// Print the public method accessors.
	out << std::endl;
	for(auto const& pair : pairs) {
		auto s = Capitalize(pair.first);
		auto type = GetType(pair.second);
		out << '\t' << type << " const& Get" << s << "() const { return " << pair.first << "; }" << std::endl;
	}

	// Print the CollectQueryName method.
	out << std::endl;
	out << "\tbool CollectQueryName(char const*& p, xstring*& q) override {" << std::endl;
	if(!nameGroups.empty()) {
		out << "\t\tswitch(*p) {" << std::endl;
		for(auto const& nameGroup : nameGroups) {
			out << "\t\tcase '" << nameGroup.first << "':" << std::endl;
			for(auto const& pair : nameGroup.second) {
				out << "\t\t\tif(memcmp(p + 1, \"" << pair.first.substr(1) << "=\", " << pair.first.size() << ") == 0) {" << std::endl;
				if(pair.second) {
					out << "\t\t\t\tif(" << pair.first << ".count >= " << pair.first << ".max) {" << std::endl;
					out << "\t\t\t\t\tthrow std::runtime_error(\"overflow of " << pair.first << "\");" << std::endl;
					out << "\t\t\t\t}" << std::endl;
					out << "\t\t\t\tq = &" << pair.first << "[" << pair.first << ".count];" << std::endl;
					out << "\t\t\t\t++" << pair.first << ".count;" << std::endl;
				} else {
					out << "\t\t\t\tq = &" << pair.first << ';' << std::endl;
				}
				out << "\t\t\t\tp += " << (pair.first.size() + 1) << ';' << std::endl;
				out << "\t\t\t\treturn true;" << std::endl;
				out << "\t\t\t}" << std::endl;
			}
			out << "\t\t\tbreak;" << std::endl;
		}
		out << "\t\t}" << std::endl;
	}
	out << "\t\treturn QueryBase::CollectQueryName(p, q);" << std::endl;
	out << "\t}" << std::endl;

	// Print the member variables.
	out << std::endl;
	out << "private:" << std::endl;
	for(auto const& pair : pairs) {
		out << '\t';
		if(pair.second) {
			out << "xvector<" << pair.second << '>';
		} else {
			out << "xstring";
		}
		out << ' ' << pair.first << ';' << std::endl;
	}
	out << "};" << std::endl;

	return rv;
}
