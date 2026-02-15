#pragma once

#include "sinful/pch.hpp"

#include "Token.hpp"

namespace Sinful::Nodes
{
	struct Node;

	struct LiteralNode { int value; };
	struct VariableNode { std::string name; };
	struct BinaryExpr { std::string op; std::unique_ptr<Node> left; std::unique_ptr<Node> right; };
	struct Assignment { std::string name; std::unique_ptr<Node> value; };
	struct PrintStmt { std::unique_ptr<Node> value; };

	using NodeData = std::variant<
		LiteralNode,
		VariableNode,
		BinaryExpr,
		Assignment,
		PrintStmt
	>;

	struct Node
	{
		NodeData data;

		template<typename T>
		bool is() const { return std::holds_alternative<T>(data); }
	};
}
