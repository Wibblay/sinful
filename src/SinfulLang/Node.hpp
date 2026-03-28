#pragma once

#include "sinful/pch.hpp"

#include "Exceptions.hpp"
#include "Token.hpp"
#include "Types.hpp"

namespace Sinful::Nodes
{
	struct Node;

	enum class BinaryOp { Add, Sub, Mul, Div, Eq, NotEq, Lt, LtEq, Gt, GtEq, And, Or };
	enum class UnaryOp  { Negate, Not };

	struct LiteralNode  { std::string value; };
	struct VariableNode { std::string name; };
	struct UnaryExpr    { UnaryOp  op; std::unique_ptr<Node> operand; };
	struct BinaryExpr   { BinaryOp op; std::unique_ptr<Node> left; std::unique_ptr<Node> right; };
	struct Assignment
	{
		std::string name;
		std::unique_ptr<Node> value;
		bool mustDeclare = false;
		Exceptions::SourceLocation location;
	};
	struct Declaration  { std::string name; Exceptions::SourceLocation location; };
	struct PrintStmt    { std::unique_ptr<Node> value; };
	struct ScopeNode    { std::vector<std::unique_ptr<Node>> statements; };

	using NodeData = std::variant<
		LiteralNode,
		VariableNode,
		BinaryExpr,
		UnaryExpr,
		Assignment,
		Declaration,
		PrintStmt,
		ScopeNode
	>;

	struct Node
	{
		NodeData data;
		Types::Type type = Types::Type::none();
		Exceptions::SourceLocation location;

		template<typename T>
		bool is() const { return std::holds_alternative<T>(data); }
	};
}
