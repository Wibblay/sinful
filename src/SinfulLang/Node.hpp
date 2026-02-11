#pragma once

#include <exception>
#include <stdexcept>
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

	Node(NodeType type, 
		std::shared_ptr<Node> left = nullptr, 
		std::shared_ptr<Node> right = nullptr) 
		: _type(type), _left(std::move(left)), _right(std::move(right)), _value("") { }

	Node(NodeType type, std::string value) 
		: _type(type), _left(nullptr), _right(nullptr), _value(std::move(value)) { }

	std::shared_ptr<Node> left() const { return _left; }
	std::shared_ptr<Node> right() const { return _right; }
	NodeType type() const { return _type; }
	const std::string& value() const 
	{ 
		if (_value.empty())
			throw std::runtime_error("node has no value");
		return _value;
	}

	void setRight(std::shared_ptr<Node> right)
	{
		if (_right != nullptr)
			throw std::runtime_error("attempted to overwrite existing right node");
		_right = right;
	}
	void setValue(std::string value) { _value = std::move(value); }
	void setOperation(const std::function<int(int, int)> operation) { _operation = operation; }

	bool is(const NodeType type) const { return _type == type; }
	inline bool is(const std::initializer_list<NodeType>& types) const
	{
		for (auto type : types)
			if (type == _type) return true;
		return false;
	}
	int operate(const int a, const int b) const { return _operation(a, b); }

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

static std::shared_ptr<Node> makeAddNode(std::shared_ptr<Node> left, std::shared_ptr<Node> right)
{
	std::shared_ptr<Node> newNode = std::make_shared<Node>(NodeType::BinExpr, std::move(left), std::move(right));
	newNode->setValue("+");
	newNode->setOperation([](int a, int b) { return a + b; });
	return newNode;
}

static std::shared_ptr<Node> makeSubtractNode(std::shared_ptr<Node> left, std::shared_ptr<Node> right)
{
	std::shared_ptr<Node> newNode = std::make_shared<Node>(NodeType::BinExpr, std::move(left), std::move(right));
	newNode->setValue("-");
	newNode->setOperation([](int a, int b) { return a - b; });
	return newNode;
}
