#include "Lexer.hpp"

using namespace Sinful::Exceptions;
using namespace Sinful::Tokens;

namespace Sinful::Lexer
{
	char Scanner::advance()
	{
		char c = source[position++];
		if (c == '\n')
		{
			line++;
			position = 0;
		}
		return c;
	}

	void Scanner::newLine(std::string_view newLine)
	{
		source = newLine;
		line++;
		position = 0;
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

	void tokeniseLine(Scanner& sc, TokenStream& tokens)
	{
		while (!sc.atEnd())
		{
			char c = sc.peek();

			if (std::isspace(c))
				sc.advance();
			else if (std::isdigit(c))
			{
				auto val = consumeNumeric(sc);
				auto loc = sc.currentLoc();
				tokens.add({ TokenType::IntLiteral, std::string(val), loc });
			}
			else if (std::isalpha(c))
			{
				auto val = consumeWord(sc);
				auto loc = sc.currentLoc();
				if (val == "print")
					tokens.add({ TokenType::Print, loc });
				else if (val == "int")
					tokens.add({ TokenType::I32Type, loc });
				else
					tokens.add({ TokenType::Variable, std::string(val), loc });
			}
			else
			{
				sc.advance();
				auto loc = sc.currentLoc();
				if (c == '/')
				{
					char c2 = sc.peek();
					if (c2 == '/')
						break;
					else
						tokens.add({ TokenType::FSlash, loc });
				}
				else
				{
					switch (c)
					{
					case '=': tokens.add({ TokenType::Equals, loc }); break;
					case '+': tokens.add({ TokenType::Plus, loc }); break;
					case '-': tokens.add({ TokenType::Minus, loc }); break;
					case '*': tokens.add({ TokenType::Star, loc }); break;
					case ';': tokens.add({ TokenType::SemiColon, loc }); break;
					case '(': tokens.add({ TokenType::LBracket, loc }); break;
					case ')': tokens.add({ TokenType::RBracket, loc }); break;
					case '{': tokens.add({ TokenType::LBrace, loc }); break;
					case '}': tokens.add({ TokenType::RBrace, loc }); break;
					default:
						throw CompilerException(Diagnostic{
							Exceptions::Diagnostic::Level::Error,
							loc,
							std::string("Unrecognised character encountered while lexing: ") + c
							});
					}
				}
			}
		}
	}
}