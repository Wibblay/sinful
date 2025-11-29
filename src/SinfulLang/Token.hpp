#pragma once

#include <string>
#include <optional>

enum class TokenType
{
	NullToken,
	SemiColon,
	
	// Data Types
	IntLiteral,

	// Operators
	Plus,
};

inline std::string TokenTypeToString(TokenType type)
{
	switch (type)
	{
	case TokenType::NullToken:   return "NullToken";
	case TokenType::SemiColon:   return "SemiColon";
	case TokenType::IntLiteral:  return "IntLiteral";
	case TokenType::Plus:        return "Plus";
	default:                     return "Unknown";
	}
}

struct Token
{
	TokenType Type;

	Token(TokenType type) : Type(type) {}
	virtual inline std::string ToString() const { return TokenTypeToString(Type); }
	virtual inline std::string GetLabel() const { return ""; }
};

struct LabelledToken : public Token
{
	std::string Label;

	LabelledToken(TokenType type, std::string label) : Token(type), Label(label) {}
	inline std::string ToString() const override { return TokenTypeToString(Type) + ": " + Label; }
	inline std::string GetLabel() const override { return Label; }
};