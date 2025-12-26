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
	static std::unique_ptr<TokenStream> tokeniseLine(const std::string& line)
	{
		std::vector<Token> tokenised;
		for (size_t i = 0; i < line.length(); i++)
		{
			if (std::isspace(line[i]) != 0)
				continue;
			else if (std::isdigit(line[i]) != 0)
			{
				auto res = parseNumeric(line, i);
				if (res)
					tokenised.emplace_back(Token(TokenType::IntLiteral, *res));
				else
					throw res.error();
			}
			else if (std::isalpha(line[i]) != 0)
			{
				auto res = parseWord(line, i);
				if (res == "print")
					tokenised.emplace_back(Token(TokenType::Print, res));
				else
					tokenised.emplace_back(Token(TokenType::Variable, res));
			}
			else if (line[i] == '+')
				tokenised.emplace_back(Token(TokenType::Plus));
			else if (line[i] == '-')
				tokenised.emplace_back(Token(TokenType::Minus));
			else if (line[i] == '*')
				tokenised.emplace_back(Token(TokenType::Star));
			else if (line[i] == ';')
				tokenised.emplace_back(Token(TokenType::SemiColon));
			else
			{
				throw std::runtime_error("Unrecognised character");
			}
		}

		return std::make_unique<TokenStream>(tokenised);
	}

	static std::expected<std::string, std::exception> parseNumeric(const std::string& line, size_t index)
	{
		std::string val;
		while (index < line.length())
		{
			if (std::isdigit(line[index]) != 0)
			{
				val += line[index];
				index++;
			}
			else if (std::isalpha(line[index]) != 0)
			{
				return std::unexpected(std::runtime_error("Variable name cannot start with numeric value"));
			}
			else
			{
				index--;
				return val;
			}
		}
		
		index--;
		return val;
	}

	static std::string parseWord(const std::string& line, size_t index)
	{
		std::string val;
		while (index < line.length())
		{
			if (std::isalnum(line[index]) != 0)
			{
				val += line[index];
				index++;
			}
			else
			{
				index--;
				return val;
			}
		}

		index--;
		return val;
	}
}
