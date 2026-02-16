#include "Token.hpp"

using namespace Sinful::Exceptions;

namespace Sinful::Tokens
{
	std::string tokenTypeToString(TokenType type)
	{
		switch (type)
		{
			using enum TokenType;
		case NullToken:		return "NullToken";
		case Variable:		return "Variable";
		case IntLiteral:	return "IntLiteral";
		case Equals:		return "Equals";
		case Plus:			return "Plus";
		case Minus:			return "Minus";
		case Star:			return "Star";
		case FSlash:		return "ForwardSlash";
		case I32Type:		return "i32";
		case SemiColon:		return "SemiColon";
		case LBracket:		return "LeftBracket";
		case RBracket:		return "RightBracket";
		case LBrace:		return "LeftBrace";
		case RBrace:		return "RightBrace";
		case Print:			return "print";
		default:            return "Unknown";
		}
	}

	Token::Token(TokenType type, SourceLocation loc) : _type(type), _location(loc)
	{
		switch (type)
		{
			using enum TokenType;
		case NullToken: case Print: case Variable: case IntLiteral:
		case I32Type:
			_lexeme = ""; break;
		case Equals:		_lexeme = "="; break;
		case Plus:			_lexeme = "+"; break;
		case Minus:			_lexeme = "-"; break;
		case Star:			_lexeme = "*"; break;
		case FSlash:		_lexeme = "/"; break;
		case SemiColon:		_lexeme = ";"; break;
		case LBracket:		_lexeme = "("; break;
		case RBracket:		_lexeme = ")"; break;
		case LBrace:		_lexeme = "{"; break;
		case RBrace:		_lexeme = "}"; break;
		default: throw std::runtime_error("Unknown default lexeme for type");
		}
	}

	const Token& TokenStream::peek(const size_t offset) const
	{
		size_t index = _current + offset;
		if (index >= _tokens.size())
			return NULL_TOKEN;
		return _tokens[index];
	}
}
