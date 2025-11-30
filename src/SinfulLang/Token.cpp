#include "Token.hpp"

void Token::defaultToken()
{
	switch (_type)
	{
	case TokenType::NullToken:
		_lexeme = "";
		break;
	case TokenType::SemiColon:
		_lexeme = ";";
		break;
	case TokenType::Plus:
		_lexeme = "+";
		break;
	case TokenType::Minus:
		_lexeme = "-";
		break;
	default:
		throw std::exception("Token type has no default constructor");
	}
}

void TokenStream::addToken(Token& token)
{
	_tokens.push_back(token);
}

const Token& TokenStream::peek(int n) const
{
	std::ptrdiff_t peekLoc = static_cast<std::ptrdiff_t>(_current) + n;
	if (peekLoc < 0 || peekLoc >= static_cast<std::ptrdiff_t>(_tokens.size()))
		return nullToken;
	return _tokens[peekLoc];
}

const Token& TokenStream::next()
{
	if (_current >= _tokens.size() - 1)
		return nullToken;
	return _tokens[++_current];
}

const Token& TokenStream::previous()
{
	if (_current <= 0)
		return nullToken;
	return _tokens[--_current];
}

