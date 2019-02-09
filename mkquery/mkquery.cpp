#include "pch.h"

namespace {
	template<typename F>
	F* Open(F& f, char const* filePath, char const* message) {
		f.open(filePath);
		if(f) {
			return &f;
		}
		std::cerr << "cannot open \"" << filePath << "\" for " << message << std::endl;
		return nullptr;
	}
}

int main(int argc, char* argv[]) {
	// Configure the input and output files.
	std::ifstream fin;
	std::istream* pin = argc > 1 ? Open(fin, argv[1], "reading") : &std::cin;
	if(!pin) {
		return 1;
	}
	std::ofstream fout;
	std::ostream* pout = argc > 2 ? Open(fout, argv[2], "writing") : &std::cout;
	if(!pout) {
		return 1;
	}

	// Read the names to capture.
	std::vector<std::string> names;
	std::copy(std::istream_iterator<std::string>(*pin), std::istream_iterator<std::string>(), std::back_inserter(names));

	// Print the CollectQuery function.
	*pout << "bool CollectQuery(char const* p) {" << std::endl;
	*pout << '}' << std::endl;
}
