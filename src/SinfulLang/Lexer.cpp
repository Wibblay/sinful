#include "Lexer.hpp"

using namespace Sinful::Tokens;

namespace Sinful::Lexer
{
	char Scanner::advance()
	{
		char c = source[position++];
		if (c == '\n')
		{
			line++;
			column = 1;
		}
		else
			column++;
		return c;
	}

	static std::string_view consumeWord(Scanner& sc)
	{
		size_t start = sc.position;
		while (std::isalnum(sc.peek())) sc.advance();
		return sc.source.substr(start, sc.position - start);
	}

	static std::string_view consumeNumeric(Scanner& sc)
	{
		size_t start = sc.position;
		while (std::isdigit(sc.peek())) sc.advance();

		if (std::isalpha(sc.peek()))
			throw std::runtime_error("Variable name cannot start with numeric value");

		return sc.source.substr(start, sc.position - start);
	}

	std::unique_ptr<TokenStream> tokeniseLine(std::string_view line, std::string filename)
	{
		std::vector<Token> tokens;
		Scanner sc{ line, filename };

		while (!sc.atEnd())
		{
			char c = sc.peek();

			if (std::isspace(c))
				sc.advance();
			else if (std::isdigit(c))
			{
				auto val = consumeNumeric(sc);
				auto loc = sc.currentLoc();
				tokens.emplace_back(TokenType::IntLiteral, std::string(val), loc);
			}
			else if (std::isalpha(c))
			{
				auto val = consumeWord(sc);
				auto loc = sc.currentLoc();
				if (val == "print")
					tokens.emplace_back(TokenType::Print, loc);
				else
					tokens.emplace_back(TokenType::Variable, std::string(val), loc);
			}
			else
			{
				sc.advance();
				auto loc = sc.currentLoc();
				switch (c)
				{
				case '+': tokens.emplace_back(TokenType::Plus, loc); break;
				case '-': tokens.emplace_back(TokenType::Minus, loc); break;
				case '*': tokens.emplace_back(TokenType::Star, loc); break;
				case ';': tokens.emplace_back(TokenType::SemiColon, loc); break;
				case '=': tokens.emplace_back(TokenType::Equals, loc); break;
				default:  throw std::runtime_error("Unrecognised character");
				}
			}
		}

		return std::make_unique<TokenStream>(std::move(tokens));
	}
}