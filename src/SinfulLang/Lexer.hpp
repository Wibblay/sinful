#pragma once

#include <vector>
#include <string>
#include <expected>
#include <exception>
#include <memory>
#include <sstream>

#include "Token.hpp"

class Lexer
{
public:
	static std::unique_ptr<TokenStream> TokeniseLine(const std::string& line)
	{
		std::vector<Token> tokenised;
		for (size_t i = 0; i < line.length(); i++)
		{
			if (std::isspace(line[i]) != 0)
				continue;
			else if (std::isdigit(line[i]) != 0)
			{
				auto res = ParseNumeric(line, i);
				if (res)
					tokenised.emplace_back(Token(TokenType::IntLiteral, *res));
				else
					throw res.error();
			}
			else if (line[i] == '+')
				tokenised.emplace_back(Token(TokenType::Plus));
			else if (line[i] == '-')
				tokenised.emplace_back(Token(TokenType::Minus));
			else if (line[i] == ';')
				tokenised.push_back(Token(TokenType::SemiColon));
			else
			{
				throw std::runtime_error("Unrecognised character");
			}
		}

		return std::make_unique<TokenStream>(tokenised);
	}

	static std::expected<std::string, std::exception> ParseNumeric(const std::string& line, size_t& index)
	{
		std::string val;
		while (index < line.length())
		{
			if (std::isdigit(line[index]) != 0)
			{
				val += line[index];
				index++;
			}
			else if (std::isspace(line[index]) != 0 || line[index] == ';')
			{
				index--;
				return val;
			}
			else
				return std::unexpected(std::runtime_error("Variable name cannot start with numeric value"));
		}

		index--;
		return val;
	}
};