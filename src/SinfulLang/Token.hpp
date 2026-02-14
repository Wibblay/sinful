#pragma once

#include <initializer_list>
#include <format>
#include <string>
#include <vector>

namespace Sinful::Tokens
{
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

	std::string_view tokenTypeToString(TokenType type);

	class Token
	{
	public:

		Token(TokenType type, std::string lexeme) : _type(type), _lexeme(std::move(lexeme)) {}
		explicit Token(TokenType type = TokenType::NullToken);

		TokenType type() const { return _type; }
		const std::string& lexeme() const { return _lexeme; }

		bool is(const TokenType type) const { return _type == type; }
		bool is(std::initializer_list<TokenType> types) const
		{
			for (auto type : types)
				if (_type == type) return true;
			return false;
		}

		std::string toString() const
		{
			if (_lexeme.empty()) return std::string(tokenTypeToString(_type));
			return std::format("{}({})", tokenTypeToString(_type), _lexeme);
		}

	private:

		TokenType _type;
		std::string _lexeme;
	};

	inline const Token NULL_TOKEN{ TokenType::NullToken, "" };

	class TokenStream
	{
	public:

		explicit TokenStream(std::vector<Token> tokens) : _tokens(std::move(tokens)), _current(0) {}

		size_t size() const { return _tokens.size(); }
		size_t currentPosition() const { return _current; }

		void add(Token& token) { _tokens.push_back(token); }

		const Token& peek(const int offset = 0) const;

		const Token& next() { return hasNext() ? _tokens[++_current] : NULL_TOKEN; }
		const Token& previous() { return hasPrevious() ? _tokens[--_current] : NULL_TOKEN; }

		bool hasNext() const { return _current < _tokens.size() - 1; }
		bool hasPrevious() const { return _current > 0; }

	private:

		std::vector<Token> _tokens;
		size_t _current;
	};
}