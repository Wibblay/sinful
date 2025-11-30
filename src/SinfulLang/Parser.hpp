#pragma once

#include <exception>
#include <expected>
#include <functional>
#include <memory>
#include <optional>
#include <variant>
#include <vector>

#include "Token.hpp"

struct Node
{
	virtual ~Node() = default;
	inline virtual int Resolve() const = 0;
};

struct LeafNode : Node
{
	int Value;

	explicit LeafNode(int value) : Value(value) {}
	int Resolve() const override { return Value; }
};

struct BinOpNode : Node
{
	std::unique_ptr<Node> Left;
	std::unique_ptr<Node> Right;
	std::function<int(int, int)> Operation;

	BinOpNode(std::unique_ptr<Node>&& left, std::unique_ptr<Node>&& right, std::function<int(int, int)> op) : Left(std::move(left)), Right(std::move(right)), Operation(op) {}
	int Resolve() const override { return Operation(Left->Resolve(), Right->Resolve()); }
};

inline std::unique_ptr<Node> AddNode(std::unique_ptr<Node>&& left, std::unique_ptr<Node>&& right)
{
	return std::make_unique<BinOpNode>(BinOpNode(std::move(left), std::move(right), [](int a, int b) { return a + b; }));
}

inline std::unique_ptr<Node> MinusNode(std::unique_ptr<Node>&& left, std::unique_ptr<Node>&& right)
{
	return std::make_unique<BinOpNode>(BinOpNode(std::move(left), std::move(right), [](int a, int b) { return a - b; }));
}

static std::unique_ptr<Node> ParseTokens(TokenStream& tokens)
{
	const Token& first = tokens.peek(0);
	if (!first.is(TokenType::IntLiteral)) throw std::exception("first token must be an integer");
	std::unique_ptr<Node> root = std::make_unique<LeafNode>(std::stoi(first.lexeme()));
	tokens.next();

	while(tokens.hasNext())
	{
		const Token& currentToken = tokens.peek(0);
		if (currentToken.is(TokenType::Plus))
		{
			const Token& nextToken = tokens.next();
			if (nextToken.is(TokenType::IntLiteral))
			{
				root = AddNode(
					std::move(root), 
					std::make_unique<LeafNode>(std::stoi(nextToken.lexeme()))
				);
				tokens.next();
			}
			else
			{
				throw std::runtime_error("+ must be followed by an int");
			}
		}
		else if (currentToken.is(TokenType::Minus))
		{
			const Token& nextToken = tokens.next();
			if (nextToken.is(TokenType::IntLiteral))
			{
				root = MinusNode(
					std::move(root),
					std::make_unique<LeafNode>(std::stoi(nextToken.lexeme()))
				);
				tokens.next();
			}
			else
			{
				throw std::runtime_error("- must be followed by an int");
			}
		}
		else if (currentToken.is(TokenType::SemiColon))
		{
			if (tokens.hasNext())
				throw std::runtime_error("semi colon must be at the end of the line");
			break;
		}
		else
			throw std::runtime_error("order must be int then operator");
	}
	if (!tokens.peekType(0, TokenType::SemiColon))
		throw std::runtime_error("line must end with a semi colon");

	return root;
}