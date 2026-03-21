#pragma once

#include "sinful/pch.hpp"

#include "Token.hpp"

namespace Sinful::Lexer
{
	struct Scanner
	{
		std::string fileName;
		std::string_view source = "";
		size_t line{};
		size_t position{};

		char peek() const { return position < source.length() ? source[position] : '\0'; }
		char advance();
		Exceptions::SourceLocation currentLoc() const { return { fileName, line - 1, position }; }
		void newLine(std::string_view newLine);
		bool atEnd() const { return position >= source.length(); }
	};

	static std::string_view consumeWord(Scanner& sc);

	static std::string_view consumeNumeric(Scanner& sc);

	void tokeniseLine(Scanner& sc, Tokens::TokenStream& tokens);
}
