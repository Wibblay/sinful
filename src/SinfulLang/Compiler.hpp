#pragma once

#include "sinful/pch.hpp"

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
			Exceptions::ErrorReporter errorReporter{};
			try
			{
				Lexer::Scanner sc{ _inputFile };
				Tokens::TokenStream tokenised{};
				while (std::getline(stream, lineBuffer))
				{
					errorReporter.cacheLine(_inputFile, lineBuffer);
					sc.newLine(lineBuffer);
					Lexer::tokeniseLine(sc, tokenised);
				}

				auto program = Parser::parseProgram(tokenised);
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
