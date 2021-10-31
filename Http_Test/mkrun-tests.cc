#include <cctype>
#include <iostream>
#include <string>

int main(int argc, char* argv[]) {
	std::cout << "#include <iostream>" << std::endl;
	std::cout << "#include <string>" << std::endl;
	std::cout << "#include \"run-tests.inl\"" << std::endl << std::endl;
	std::cout << "using namespace Http_Test;" << std::endl << std::endl;
	std::cout << "int main(int argc, char* argv[]) {" << std::endl;
	std::cout << "\ttry {" << std::endl;
	std::cout << "\t\tstd::cout << \"starting\" << std::endl;" << std::endl;
	std::string className, s, cleanUp, initializeMethod;
	while (std::getline(std::cin, s)) {
		while (std::isspace(s.back())) {
			s.pop_back();
		}
		switch (s[0]) {
		case 'C':
			initializeMethod.clear();
			if (!cleanUp.empty()) {
				std::cout << "\t\tstd::cout << \"" << className << "::" << cleanUp << "\" << std::endl;" << std::endl;
				std::cout << "\t\t" << className << "::" << cleanUp << "();" << std::endl;
				cleanUp.clear();
			}
			className = s.substr(1);
			std::cout << "\t\t" << className << ' ' << className << "_;" << std::endl;
			break;
		case 'M':
			std::cout << "\t\tstd::cout << \"" << className << "::" << s.substr(1) << "\" << std::endl;" << std::endl;
			if (!initializeMethod.empty()) {
				std::cout << "\t\t" << className << "_." << initializeMethod << "();" << std::endl;
			}
			std::cout << "\t\t" << className << "_." << s.substr(1) << "();" << std::endl;
			break;
		case 'N':
			std::cout << "\t\tstd::cout << \"" << className << "::" << s.substr(1) << "\" << std::endl;" << std::endl;
			std::cout << "\t\t" << className << "::" << s.substr(1) << "();" << std::endl;
			break;
		case 'L':
			cleanUp = s.substr(1);
			break;
		case 'I':
			initializeMethod = s.substr(1);
			break;
		}
	}
	std::cout << "\t} catch(std::exception const& ex) {" << std::endl;
	std::cout << "\t\tstd::cout << \"unexpected exception:  \" << ex.what() << std::endl;" << std::endl;
	std::cout << "\t}" << std::endl;
	std::cout << "\treturn 0;" << std::endl;
	std::cout << "}" << std::endl;
	return 0;
}
