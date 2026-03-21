#pragma once

#include "sinful//pch.hpp"

#include "Exceptions.hpp"
#include "Types.hpp"

namespace Sinful::Tokens
{
	enum class TokenType
	{
		NullToken,
		
		// Data
		Variable,
		IntLiteral,

		// Operators
		Equals,
		Plus,
		Minus,
		Star,
		FSlash,
		Exclamation,
		ExclEquals,
		LessThan,
		GreaterThan,
		LeOrEqual,
		GrOrEqual,
		DubEquals,

		// Types
		I32Type,
		BoolType,

		// Syntactic
		SemiColon,
		LBracket,
		RBracket,
		LBrace,
		RBrace,

		// Keywords
		Print,
		True,
		False
	};

	std::string tokenTypeToString(TokenType type);

	Types::Type convertTypeToken(TokenType type);

	class Token
	{
	public:

		Token(TokenType type, std::string lexeme, Exceptions::SourceLocation loc) : _type(type), _lexeme(std::move(lexeme)), _location(loc) {}
		Token(TokenType type = TokenType::NullToken, Exceptions::SourceLocation loc = {"", 0, 0});

		TokenType type() const { return _type; }
		const std::string& lexeme() const { return _lexeme; }
		const Exceptions::SourceLocation& location() const { return _location; }

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
		Exceptions::SourceLocation _location;
	};

	inline const Token NULL_TOKEN{ TokenType::NullToken, {"", 0, 0} };

	class TokenStream
	{
	public:

		TokenStream() : _tokens({}), _current(0) {}
		explicit TokenStream(std::vector<Token> tokens) : _tokens(std::move(tokens)), _current(0) {}

		size_t size() const { return _tokens.size(); }
		size_t currentPosition() const { return _current; }

		void add(Token token) { _tokens.push_back(std::move(token)); }

		const Token& peek(const size_t offset = 0) const;

		const Token& next() { return hasNext() ? _tokens[++_current] : NULL_TOKEN; }
		bool hasNext() const { return _current < _tokens.size() - 1; }

	private:

		std::vector<Token> _tokens;
		size_t _current;
	};
}