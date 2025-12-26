#pragma once

#include <exception>
#include <functional>
#include <initializer_list>
#include <memory>
#include <string>

#include "Token.hpp"

enum class NodeType
{
	Variable,
	IntLiteral,

	BinExpr,
	Assignment,
	Print,
};

class Node
{
public:

	Node(NodeType type, std::shared_ptr<Node>&& left, std::shared_ptr<Node>&& right) : _type(type),
		_left(std::move(left)), _right(std::move(right)) { }
	Node(NodeType type, std::shared_ptr<Node>&& left) : _type(type), _left(std::move(left)), _right(nullptr) { }
	Node(NodeType type) : _type(type), _left(nullptr), _right(nullptr) { }
	Node(NodeType type, std::string& value) : _type(type), _left(nullptr), _right(nullptr), _value(value) { }

	std::shared_ptr<Node> left() const { return _left; }
	std::shared_ptr<Node> right() const { return _right; }
	const NodeType type() const { return _type; }
	const std::string& value() const { return _value.size() > 0 ? _value : throw std::runtime_error("node has no value"); }

	inline void setRight(std::shared_ptr<Node>&& right);
	void setOperation(std::function<int(int, int)> operation) { _operation = operation; }

	const bool is(const NodeType type) const { return _type == type; }
	inline const bool is(const std::initializer_list<NodeType>& types) const;

private:

	NodeType _type;
	std::shared_ptr<Node> _left;
	std::shared_ptr<Node> _right;
	std::string _value;
	std::function<int(int, int)> _operation;
};

inline static const NodeType tokenTypeToNodeType(TokenType type)
{
	switch (type)
	{
	case TokenType::Variable:	 return NodeType::Variable;
	case TokenType::IntLiteral:  return NodeType::IntLiteral;
	default:                     throw std::runtime_error("token type has no direct node equivalent");
	}
}

static std::shared_ptr<Node> makeAddNode(std::shared_ptr<Node>&& left, std::shared_ptr<Node>&& right)
{
	std::shared_ptr<Node> newNode = std::make_unique<Node>(NodeType::BinExpr, std::move(left), std::move(right));
	newNode->setOperation([](int a, int b) { return a + b; });
	return newNode;
}

static std::shared_ptr<Node> makeSubtractNode(std::shared_ptr<Node>&& left, std::shared_ptr<Node>&& right)
{
	std::shared_ptr<Node> newNode = std::make_unique<Node>(NodeType::BinExpr, std::move(left), std::move(right));
	newNode->setOperation([](int a, int b) { return a - b; });
	return newNode;
}
