#pragma once

#include <initializer_list>
#include <string>
#include <vector>

enum class TokenType
{
	NullToken,
	SemiColon,
	
	// Data
	Variable,
	IntLiteral,

	// Operators
	Equals,
	Plus,
	Minus,
	Star,

	// Keywords
	Print,
};

class Token
{
public:

	Token(TokenType type, std::string& lexeme) : _type(type), _lexeme(lexeme) {}
	Token(TokenType type = TokenType::NullToken) : _type(type) { defaultToken(); }

	const TokenType type() const { return _type; }
	const std::string& lexeme() const { return _lexeme; }

	const bool is(const TokenType type) const { return _type == type; }
	inline const bool is(const std::initializer_list<TokenType>& types) const;
	inline const std::string& toString() const;

private:

	TokenType _type;
	std::string _lexeme;

	void defaultToken();
};

static const Token NULL_TOKEN;

class TokenStream
{
public:

	TokenStream(std::vector<Token>& tokens) : _tokens(tokens), _current(0) {}

	const size_t size() const { return _tokens.size(); }
	const size_t currentPosition() const { return _current; }

	void addToken(Token& token);

	const Token& peek(const int n) const;
	const bool peekType(int n, const TokenType type) const { return peek(n).is(type); }
	const bool peekType(const int n, const std::initializer_list<TokenType>& types) const { return peek(n).is(types); }

	const Token& next() { return _current >= _tokens.size() - 1 ? NULL_TOKEN : _tokens[++_current]; }
	const Token& previous() { return _current < 0 ? NULL_TOKEN : _tokens[--_current]; }

	const bool hasNext() const { return _current < _tokens.size() - 1; }
	const bool hasPrevious() const { return _current > 0; }

private:

	std::vector<Token> _tokens;
	size_t _current;
};
