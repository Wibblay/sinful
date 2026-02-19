#pragma once

#include "sinful/pch.hpp"

#include "Exceptions.hpp"
#include "Token.hpp"

namespace Sinful::Nodes
{
	struct Node;

	struct LiteralNode { int value; };
	struct VariableNode { std::string name; Exceptions::SourceLocation location; };
	struct BinaryExpr { std::string op; std::unique_ptr<Node> left; std::unique_ptr<Node> right; };
	struct Declaration { std::string name; Exceptions::SourceLocation location; };
	struct Assignment { std::string name; std::unique_ptr<Node> value; };
	struct PrintStmt { std::unique_ptr<Node> value; };
	struct ScopeNode { std::vector<std::unique_ptr<Node>> statements; };

	using NodeData = std::variant<
		LiteralNode,
		VariableNode,
		BinaryExpr,
		Declaration,
		Assignment,
		PrintStmt,
		ScopeNode
	>;

	struct Node
	{
		NodeData data;

		template<typename T>
		bool is() const { return std::holds_alternative<T>(data); }
	};
}
