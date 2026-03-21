#pragma once

#include "sinful/pch.hpp"

#include "Exceptions.hpp"
#include "Token.hpp"
#include "Types.hpp"

namespace Sinful::Nodes
{
	struct Node;

	struct LiteralNode  { std::string value; };
	struct VariableNode { std::string name; };
	struct BinaryExpr   { std::string op; std::unique_ptr<Node> left; std::unique_ptr<Node> right; };
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
