#include "Token.hpp"

inline static const std::string& tokenTypeToString(TokenType type)
{
	switch (type)
	{
	case TokenType::NullToken:   return "NullToken";
	case TokenType::SemiColon:   return "SemiColon";
	case TokenType::Variable:	 return "Variable";
	case TokenType::IntLiteral:  return "IntLiteral";
	case TokenType::Equals:		 return "Equals";
	case TokenType::Plus:        return "Plus";
	case TokenType::Minus:		 return "Minus";
	case TokenType::Star:		 return "Star";
	case TokenType::Print:		 return "Print";
	default:                     return "NoStringMapping";
	}
}

void Token::defaultToken()
{
	switch (_type)
	{
	case TokenType::NullToken:
	case TokenType::Print:
		_lexeme = "";
		break;
	case TokenType::SemiColon:
		_lexeme = ";";
		break;
	case TokenType::Equals:
		_lexeme = "=";
		break;
	case TokenType::Plus:
		_lexeme = "+";
		break;
	case TokenType::Minus:
		_lexeme = "-";
		break;
	case TokenType::Star:
		_lexeme = "*";
		break;
	default:
		throw std::exception("Token type has no default constructor");
	}
}

inline const bool Token::is(const std::initializer_list<TokenType>& types) const
{
	for (auto type : types)
		if (_type == type) return true;
	return false;
}

inline const std::string& Token::toString() const
{
	return tokenTypeToString(_type) + _lexeme == "" ? "" : "(" + _lexeme + ")";
}

void TokenStream::addToken(Token& token)
{
	_tokens.push_back(token);
}

const Token& TokenStream::peek(const int n) const
{
	std::ptrdiff_t peekLoc = static_cast<std::ptrdiff_t>(_current) + n;
	if (peekLoc < 0 || peekLoc >= static_cast<std::ptrdiff_t>(_tokens.size()))
		return NULL_TOKEN;
	return _tokens[peekLoc];
}
