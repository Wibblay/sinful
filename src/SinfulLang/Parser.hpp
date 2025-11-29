#pragma once

#include <exception>
#include <expected>
#include <optional>
#include <vector>

#include "Token.hpp"

struct Node
{
	std::optional<Node> Left;
	std::optional<Node> Right;

	Node(std::optional<Node> left, std::optional<Node> right) : Left(left), Right(right) {}
	Node(Node left) : Node(left, std::nullopt) {}
	Node() : Node(std::nullopt, std::nullopt) {}
};

struct IntLiteralNode : public Node
{
	int Value;

	IntLiteralNode(int value, std::optional<Node> left, std::optional<Node> right) : Node(left, right), Value(value) {}
	IntLiteralNode(int value, Node left) : Node(left, std::nullopt), Value(value) {}
	IntLiteralNode(int value) : Node(std::nullopt, std::nullopt), Value(value) {}
};

static Node ParseTokens(const std::vector<std::unique_ptr<Token>>& tokens)
{
	if (tokens.size() == 0) throw;
	if (tokens[0].Type != TokenType::IntLiteral) throw;
	Node()
	for (size_t i = 0; i < tokens.size(); i++)
	{

	}
}