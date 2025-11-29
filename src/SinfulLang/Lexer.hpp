#pragma once

#include <vector>
#include <string>
#include <expected>
#include <exception>
#include <iostream>
#include <memory>

#include "Token.hpp"

class Lexer
{
public:
	static std::vector<std::unique_ptr<Token>> TokeniseLine(const std::string& line)
	{
		std::vector<std::unique_ptr<Token>> tokenised;
		for (size_t i = 0; i < line.length(); i++)
		{
			if (std::isspace(line[i]) != 0)
				continue;
			else if (std::isdigit(line[i]) != 0)
			{
				auto res = ParseNumeric(line, i);
				if (res)
					tokenised.push_back(std::make_unique<LabelledToken>(TokenType::IntLiteral, *res));
				else
					throw res.error();
			}
			else if (line[i] == '+')
				tokenised.push_back(std::make_unique<Token>(TokenType::Plus));
			else if (line[i] == ';')
				tokenised.push_back(std::make_unique<Token>(TokenType::SemiColon));
			else
			{
				std::cerr << "Unrecognised character" << std::endl;
				throw;
			}
		}

		return tokenised;
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
				return std::unexpected(std::exception("Variable name cannot start with numeric value"));
		}

		index--;
		return val;
	}
};