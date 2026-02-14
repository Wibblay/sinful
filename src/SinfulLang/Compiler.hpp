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

namespace Sinful
{
	class Compiler
	{
	public:
		Compiler(std::string inputFile, std::string outputFile) : _inputFile(inputFile),
			_outputFile(outputFile)
		{
			_generator = new AsmGeneration::Generator();
		};

		virtual ~Compiler()
		{
			delete _generator;
		}

		void Compile() const
		{
			std::cout << "Compiling..." << std::endl;
			std::ifstream stream(_inputFile);

			std::string lineBuffer;
			std::istringstream lineStream;
			std::vector<std::unique_ptr<Nodes::Node>> program;
			while (std::getline(stream, lineBuffer))
			{
				if (lineBuffer.length() == 0)
					continue;
				auto tokens = Lexer::tokeniseLine(lineBuffer);
				std::unique_ptr<Nodes::Node> tree = Parser::parseStatement(*tokens);
				program.emplace_back(std::move(tree));
			}

			for (auto& statement : program)
				_generator->generateStatementAsm(*statement);

			std::ofstream out(_outputFile);
			if (!out)
				throw std::runtime_error("Failed to open assembly output file: " + _outputFile);
			out << _generator->generateFinal();
		}

	private:

		std::string _inputFile;
		std::string _outputFile;
		AsmGeneration::Generator* _generator;
	};
}
