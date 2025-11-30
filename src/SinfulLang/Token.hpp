#pragma once

#include <string>
#include <vector>

enum class TokenType
{
	NullToken,
	SemiColon,
	
	// Data Types
	IntLiteral,

	// Operators
	Plus,
};

inline const std::string& tokenTypeToString(TokenType type)
{
	switch (type)
	{
	case TokenType::NullToken:   return "NullToken";
	case TokenType::SemiColon:   return "SemiColon";
	case TokenType::IntLiteral:  return "IntLiteral";
	case TokenType::Plus:        return "Plus";
	default:                     return "NoStringMapping";
	}
}

class Token
{
public:

	Token(TokenType type, std::string lexeme) : _type(type), _lexeme(lexeme) {}
	Token(TokenType type = TokenType::NullToken) : _type(type) { defaultToken(); }

	inline const TokenType type() const { return _type; }
	inline const std::string& lexeme() const { return _lexeme; }

	inline const bool is(TokenType compareType) const { return _type == compareType; }
	inline const std::string& toString() const { return tokenTypeToString(_type) + "(" + _lexeme + ")"; }

private:

	TokenType _type;
	std::string _lexeme;

	void defaultToken();
};

class TokenStream
{
public:

	TokenStream(std::vector<Token>& tokens) : _tokens(tokens), _current(0) {}

	void addToken(Token& token);

	const Token& peek(int n) const;
	inline const bool peekType(int n, TokenType type) const { return peek(n).is(type); }
	const Token& next();
	const Token& previous();
	inline const bool hasNext() const { return _current < _tokens.size() - 1; }
	inline const bool hasPrevious() const { return _current >= 0; }

private:

	std::vector<Token> _tokens;
	size_t _current;
};

static const Token nullToken{};
