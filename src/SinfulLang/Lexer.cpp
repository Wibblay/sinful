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
		while (std::isalnum(sc.peek()) || sc.peek() == '_') sc.advance();
		return sc.source.substr(start, sc.position - start);
	}

	static std::string_view consumeNumeric(Scanner& sc)
	{
		size_t start = sc.position;
		while (std::isdigit(sc.peek())) sc.advance();

		if (std::isalpha(sc.peek())) throw CompilerException(Diagnostic{
			Exceptions::Diagnostic::Level::Error,
			sc.currentLoc(),
			"Variable name cannot start with numeric value"
		});

		return sc.source.substr(start, sc.position - start);
	}

	void tokeniseLine(Scanner& sc, TokenStream& tokens)
	{
		while (!sc.atEnd())
		{
			auto c = sc.peek();

			if (std::isspace(c))
				sc.advance();
			else if (std::isdigit(c))
			{
				auto val = consumeNumeric(sc);
				auto loc = sc.currentLoc();
				tokens.add({ TokenType::IntLiteral, std::string(val), loc });
			}
			else if (std::isalpha(c) || c == '_')
			{
				auto val = consumeWord(sc);
				auto loc = sc.currentLoc();
				if (val == "print")
					tokens.add({ TokenType::Print, loc });
				else if (val == "i32")
					tokens.add({ TokenType::I32Type, loc });
				else if (val == "bool")
					tokens.add({ TokenType::BoolType, loc });
				else if (val == "true")
					tokens.add({ TokenType::True, loc });
				else if (val == "false")
					tokens.add({ TokenType::False, loc });
				else
					tokens.add({ TokenType::Variable, std::string(val), loc });
			}
			else
			{
				sc.advance();
				auto loc = sc.currentLoc();
				auto c2 = sc.peek();
				if (c == '/')
				{
					if (c2 == '/')
						break;
					else
						tokens.add({ TokenType::FSlash, loc });
				}
				else if (c == '=')
				{
					if (c2 == '=')
					{
						tokens.add({ TokenType::DubEquals, loc });
						sc.advance();
					}
					else
						tokens.add({ TokenType::Equals, loc });
				}
				else if (c == '!')
				{
					if (c2 == '=')
					{
						tokens.add({ TokenType::ExclEquals, loc });
						sc.advance();
					}
					else
						tokens.add({ TokenType::Exclamation, loc });
				}
				else if (c == '<')
				{
					if (c2 == '=')
					{
						tokens.add({ TokenType::LeOrEqual, loc });
						sc.advance();
					}
					else
						tokens.add({ TokenType::LessThan, loc });
				}
				else if (c == '>')
				{
					if (c2 == '=')
					{
						tokens.add({ TokenType::GrOrEqual, loc });
						sc.advance();
					}
					else
						tokens.add({ TokenType::GreaterThan, loc });
				}
				else if (c == '&')
				{
					if (c2 == '&')
					{
						tokens.add({ TokenType::AmpAmp, loc });
						sc.advance();
					}
					else
						throw CompilerException(Diagnostic{
							Exceptions::Diagnostic::Level::Error,
							loc,
							std::string("Unrecognised character encountered while lexing: ") + c
						});
				}
				else if (c == '|')
				{
					if (c2 == '|')
					{
						tokens.add({ TokenType::PipePipe, loc });
						sc.advance();
					}
					else
						throw CompilerException(Diagnostic{
							Exceptions::Diagnostic::Level::Error,
							loc,
							std::string("Unrecognised character encountered while lexing: ") + c
						});
				}
				else
				{
					switch (c)
					{
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