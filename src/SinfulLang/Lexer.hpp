#pragma once

#include <vector>
#include <string>
#include <expected>
#include <exception>
#include <memory>
#include <sstream>

#include "Token.hpp"

namespace Sinful::Lexer
{
	struct Scanner
	{
		std::string_view source;
		size_t position = 0;

		char peek() const { return position < source.length() ? source[position] : '\0'; }
		char advance() { return source[position++]; }
		bool atEnd() const { return position >= source.length(); }
	};

	static std::string_view consumeWord(Scanner& sc);

	static std::string_view consumeNumeric(Scanner& sc);

	std::unique_ptr<Tokens::TokenStream> tokeniseLine(std::string_view line);
}
