#include <iostream>
#include <string>

#include "SinfulLang\Compiler.hpp"

int main(int argc, char* argv[])
{
	if (argc != 2)
	{
		std::cerr << "Received wrong number of arguments" << std::endl;
		std::cerr << "Correct usage is './SinfulApp.exe <filename.sin>'" << std::endl;
		throw;
	}

	std::string s(argv[1]);
	if (s.length() <= 4 || s.compare(s.length() - 4, 4, ".sin") != 0)
	{
		std::cerr << "Input file is not a valid SinfulLang name" << std::endl;
		std::cerr << "File name must end with '.sin'" << std::endl;
	}

	Compiler compiler("../../../SinfulScripts/" + s, "../../assembly/" + s.substr(0, s.length() - 3) + "asm");
	compiler.Compile();
	
	return EXIT_SUCCESS;
}
