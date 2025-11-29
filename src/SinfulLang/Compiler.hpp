#pragma once

#include <string>
#include <iostream>
#include <fstream>
#include <memory>
#include <vector>

#include "Generator.hpp"
#include "Lexer.hpp"
#include "Token.hpp"

class Compiler
{
public:
	Compiler(std::string inputFile, std::string outputFile) : _inputFile(inputFile), _outputFile(outputFile) { };
	virtual ~Compiler() = default;

	void Compile() const
	{
		std::cout << "Compiling..." << std::endl;
		std::ifstream stream(_inputFile);

		std::string lineBuffer;
		while (std::getline(stream, lineBuffer))
		{
			std::cout << lineBuffer << std::endl;
			if (lineBuffer.length() == 0)
				continue;
			std::vector<std::unique_ptr<Token>> tokens = Lexer::TokeniseLine(lineBuffer);
			Generator::WriteAsm(_outputFile, *tokens[0]);
		}
	}

private:
	std::string _inputFile;
	std::string _outputFile;
};