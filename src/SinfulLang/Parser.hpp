#pragma once

#include <exception>
#include <expected>
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

	BinOpNode(std::unique_ptr<Node>&& left, std::unique_ptr<Node>&& right) : Left(std::move(left)), Right(std::move(right)) {}
	virtual ~BinOpNode() = default;
	virtual int Operate(int left, int right) const = 0;
	int Resolve() const override { return Operate(Left->Resolve(), Right->Resolve()); }
};

struct AddNode : BinOpNode
{
	AddNode(std::unique_ptr<Node>&& left, std::unique_ptr<Node>&& right) : BinOpNode(std::move(left), std::move(right)) {}
	int Operate(int left, int right) const override { return left + right; }
};


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
				root = std::make_unique<AddNode>(
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