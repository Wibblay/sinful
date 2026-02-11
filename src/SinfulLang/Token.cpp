#include "Token.hpp"

std::string_view tokenTypeToString(TokenType type)
{
	switch (type)
	{
		using enum TokenType;
		case NullToken:		return "NullToken";
		case SemiColon:		return "SemiColon";
		case Variable:		return "Variable";
		case IntLiteral:	return "IntLiteral";
		case Equals:		return "Equals";
		case Plus:			return "Plus";
		case Minus:			return "Minus";
		case Star:			return "Star";
		case Print:			return "Print";
		default:            return "Unknown";
	}
}

Token::Token(TokenType type) : _type(type)
{
	switch (type)
	{
		using enum TokenType;
		case NullToken: case Print: case Variable: case IntLiteral:
			_lexeme = ""; break;
		case SemiColon: _lexeme = ";"; break;
		case Equals:    _lexeme = "="; break;
		case Plus:      _lexeme = "+"; break;
		case Minus:     _lexeme = "-"; break;
		case Star:      _lexeme = "*"; break;
		default: throw std::runtime_error("Unknown default lexeme for type");
	}
}

const Token& TokenStream::peek(const int offset) const
{
	int index = _current + offset;
	if (index < 0 || index >= _tokens.size())
		return NULL_TOKEN;
	return _tokens[index];
}
