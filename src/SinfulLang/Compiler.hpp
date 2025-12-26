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
		std::vector<std::shared_ptr<Node>> program;
		while (std::getline(stream, lineBuffer))
		{
			if (lineBuffer.length() == 0)
				continue;
			auto tokens = Lexer::tokeniseLine(lineBuffer);
			std::shared_ptr<Node> tree = Parser::parseTokens(*tokens);
			program.emplace_back(tree);
		}
	}

private:
	std::string _inputFile;
	std::string _outputFile;
};