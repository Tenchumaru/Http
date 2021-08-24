#include <cctype>
#include <iostream>
#include <string>

int main(int argc, char* argv[]) {
	std::cout << "#include <cctype>" << std::endl;
	std::cout << "#include <iostream>" << std::endl;
	std::cout << "#include <string>" << std::endl << std::endl;
	std::cout << "#include \"run-tests.inl\"" << std::endl << std::endl;
	std::cout << "using namespace Http_Test;" << std::endl << std::endl;
	std::cout << "int main(int argc, char* argv[]) {" << std::endl;
	std::cout << "try {" << std::endl;
	std::string className, s, cleanUp, initializeMethod;
	while (std::getline(std::cin, s)) {
		while (std::isspace(s.back())) {
			s.pop_back();
		}
		switch (s[0]) {
		case 'C':
			initializeMethod.clear();
			if (!cleanUp.empty()) {
				std::cout << "\tstd::cout << \"" << className << "::" << cleanUp << "\" << std::endl;" << std::endl;
				std::cout << '\t' << className << "::" << cleanUp << "();" << std::endl;
				cleanUp.clear();
			}
			className = s.substr(1);
			std::cout << '\t' << className << ' ' << className << "_;" << std::endl;
			break;
		case 'M':
			std::cout << "\tstd::cout << \"" << className << "::" << s.substr(1) << "\" << std::endl;" << std::endl;
			if (!initializeMethod.empty()) {
				std::cout << '\t' << className << "_." << initializeMethod << "();" << std::endl;
			}
			std::cout << '\t' << className << "_." << s.substr(1) << "();" << std::endl;
			break;
		case 'N':
			std::cout << "\tstd::cout << \"" << className << "::" << s.substr(1) << "\" << std::endl;" << std::endl;
			std::cout << '\t' << className << "::" << s.substr(1) << "();" << std::endl;
			break;
		case 'L':
			cleanUp = s.substr(1);
			break;
		case 'I':
			initializeMethod = s.substr(1);
			break;
		}
	}
	std::cout << "} catch(std::exception const& ex) {" << std::endl;
	std::cout << "std::cout << \"unexpected exception:  \" << ex.what() << std::endl;" << std::endl;
	std::cout << "}" << std::endl;
	std::cout << "	return 0;" << std::endl;
	std::cout << "}" << std::endl;
	return 0;
}
