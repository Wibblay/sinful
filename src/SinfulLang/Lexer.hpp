#pragma once

#include <vector>
#include <string>
#include <expected>
#include <exception>
#include <memory>
#include <sstream>

#include "Token.hpp"

namespace Lexer
{
	struct Scanner
	{
		std::string_view source;
		size_t position = 0;

		char peek() const { return position < source.length() ? source[position] : '\0'; }
		char advance() { return source[position++]; }
		bool atEnd() const { return position >= source.length(); }
	};

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

	static std::unique_ptr<TokenStream> tokeniseLine(std::string_view line)
	{
		std::vector<Token> tokens;
		Scanner sc{ line };

		while (!sc.atEnd())
		{
			char c = sc.peek();

			if (std::isspace(c))
				sc.advance();
			else if (std::isdigit(c))
			{
				auto val = consumeNumeric(sc);
				tokens.emplace_back(TokenType::IntLiteral, std::string(val));
			}
			else if (std::isalpha(c))
			{
				auto val = consumeWord(sc);
				if (val == "print")
					tokens.emplace_back(TokenType::Print);
				else
					tokens.emplace_back(TokenType::Variable, std::string(val));
			}
			else
			{
				sc.advance();
				switch (c)
				{
					case '+': tokens.emplace_back(TokenType::Plus); break;
					case '-': tokens.emplace_back(TokenType::Minus); break;
					case '*': tokens.emplace_back(TokenType::Star); break;
					case ';': tokens.emplace_back(TokenType::SemiColon); break;
					case '=': tokens.emplace_back(TokenType::Equals); break;
					default:  throw std::runtime_error("Unrecognised character");
				}
			}
		}

		return std::make_unique<TokenStream>(std::move(tokens));
	}
}
