#pragma once

#include "sinful/pch.hpp"

#include "Token.hpp"

namespace Sinful::Lexer
{
	struct Scanner
	{
		std::string_view source;
		std::string fileName;
		size_t position = 0;
		size_t line = 1;
		size_t column = 1;

		char peek() const { return position < source.length() ? source[position] : '\0'; }
		char advance();
		Exceptions::SourceLocation currentLoc() const { return { fileName, line, column }; }
		bool atEnd() const { return position >= source.length(); }
	};

	static std::string_view consumeWord(Scanner& sc);

	static std::string_view consumeNumeric(Scanner& sc);

	std::unique_ptr<Tokens::TokenStream> tokeniseLine(std::string_view line, std::string filename);
}
