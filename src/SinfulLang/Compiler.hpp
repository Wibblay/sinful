#pragma once

#include <string>
#include <iostream>
#include <fstream>
#include <memory>
#include <sstream>
#include <vector>

#include "Generator.hpp"
#include "Lexer.hpp"
#include "Parser.hpp"
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
		std::istringstream lineStream;
		while (std::getline(stream, lineBuffer))
		{
			if (lineBuffer.length() == 0)
				continue;
			auto tokens = Lexer::TokeniseLine(lineBuffer);
			std::unique_ptr<Node> root = ParseTokens(*tokens);
			Generator::WriteAsm(_outputFile, root->Resolve());
		}
	}

private:
	std::string _inputFile;
	std::string _outputFile;
};