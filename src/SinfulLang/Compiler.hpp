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

		int Compile() const
		{
			std::cout << "Compiling..." << std::endl;
			std::ifstream stream(_inputFile);

			std::string lineBuffer;
			std::istringstream lineStream;
			std::vector<std::unique_ptr<Nodes::Node>> program;
			Exceptions::ErrorReporter errorReporter{};
			try
			{
				while (std::getline(stream, lineBuffer))
				{
					errorReporter.cacheLine(_inputFile, lineBuffer);
					auto tokens = Lexer::tokeniseLine(lineBuffer, _inputFile);
					std::unique_ptr<Nodes::Node> tree = Parser::parseStatement(*tokens);
					program.emplace_back(std::move(tree));
				}

				for (auto& statement : program)
					_generator->generateStatementAsm(*statement);
			}
			catch (const Exceptions::CompilerException& e)
			{
				errorReporter.handleException(e);
				return EXIT_FAILURE;
			}
			catch (const std::exception& e)
			{
				std::cerr << "Internal Compiler Error: " << e.what() << "\n";
				return EXIT_FAILURE;
			}

			std::ofstream out(_outputFile);
			if (!out)
				throw std::runtime_error("Failed to open assembly output file: " + _outputFile);
			out << _generator->generateFinal();
			return EXIT_SUCCESS;
		}

	private:

		std::string _inputFile;
		std::string _outputFile;
		AsmGeneration::Generator* _generator;
	};
}
