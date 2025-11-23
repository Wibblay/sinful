#pragma once

#include <string>
#include <iostream>

class Compiler
{
public:
	Compiler(std::string inputFile) : _inputFile(inputFile) { };
	virtual ~Compiler() = default;

	void Compile()
	{
		std::cout << "Compiling..." << std::endl;
	}

private:
	std::string _inputFile;
};