#pragma once

#include <exception>
#include <expected>
#include <functional>
#include <memory>
#include <optional>
#include <unordered_set>
#include <variant>
#include <vector>

#include "Node.hpp"
#include "Token.hpp"

//struct Node
//{
//	virtual ~Node() = default;
//	virtual int resolve() const = 0;
//
//	virtual const bool hasChild() const { return false; }
//	virtual Node* getRight() const { throw std::runtime_error("cannot call getright on childless node type"); }
//	virtual void setRight(std::unique_ptr<Node>&& node) { throw std::runtime_error("cannot call setright on childless node type"); }
//	virtual const int getPriority() const { return 0; }
//};
//
//struct LeafNode : Node
//{
//	int Value;
//
//	explicit LeafNode(int value) : Value(value) {}
//	int resolve() const override { return Value; }	
//};
//
//struct BinOpNode : Node
//{
//	std::unique_ptr<Node> Left;
//	std::unique_ptr<Node> Right;
//	std::function<int(int, int)> Operation;
//	const int Priority;
//
//	BinOpNode(std::unique_ptr<Node>&& left, std::unique_ptr<Node>&& right, std::function<int(int, int)> op, int priority = 1) : Left(std::move(left)), Right(std::move(right)), Operation(op), Priority(priority) {}
//	int resolve() const override { return Operation(Left->resolve(), Right->resolve()); }
//	const bool hasChild() const override { return true; }
//	Node* getRight() const { return Right.get(); }
//	void setRight(std::unique_ptr<Node>&& node) override { Right = std::move(node); }
//};
//
//inline std::unique_ptr<Node> AddNode(std::unique_ptr<Node>&& left, std::unique_ptr<Node>&& right)
//{
//	return std::make_unique<BinOpNode>(BinOpNode(std::move(left), std::move(right), [](int a, int b) { return a + b; }, 1));
//}
//
//inline std::unique_ptr<Node> SubtractNode(std::unique_ptr<Node>&& left, std::unique_ptr<Node>&& right)
//{
//	return std::make_unique<BinOpNode>(BinOpNode(std::move(left), std::move(right), [](int a, int b) { return a - b; }, 1));
//}
//
//inline std::unique_ptr<Node> MultiplyNode(std::unique_ptr<Node>&& left, std::unique_ptr<Node>&& right)
//{
//	return std::make_unique<BinOpNode>(BinOpNode(std::move(left), std::move(right), [](int a, int b) { return a * b; }, 2));
//}

namespace Parser
{
	static std::shared_ptr<Node> parseTokens(TokenStream& tokens)
	{
		size_t numTokens = tokens.size();
		if (numTokens == 0)
			throw std::runtime_error("token stream is empty");
		std::shared_ptr<Node> root;
		std::shared_ptr<Node> currentNode;
		while (tokens.currentPosition() <= numTokens - 2)
		{
			const Token& currentToken = tokens.peek(0);
			if (currentToken.is(TokenType::Equals))
			{
				if (!tokens.peekType(-1, TokenType::Variable))
					throw std::runtime_error("can only assign to variables");
				root = std::make_shared<Node>(NodeType::Assignment, std::move(root));
				currentNode = std::make_shared<Node>(root);
			}
			else if (currentToken.is({ TokenType::IntLiteral, TokenType::Variable }))
			{
				if (tokens.currentPosition() == 0)
				{
					root = std::make_shared<Node>(tokenTypeToNodeType(currentToken.type()), currentToken.lexeme());
					currentNode = std::make_shared<Node>(root);
				}
				else
				{
					if (currentNode->is({ NodeType::IntLiteral, NodeType::Variable }))
						throw std::runtime_error("no operator applied to constant/variable pair");
					currentNode->setRight(std::make_shared<Node>(tokenTypeToNodeType(currentToken.type()), currentToken.lexeme()));
				}
			}
			else if (currentToken.is(TokenType::Print))
			{
				if (!tokens.currentPosition() == 0)
					throw std::runtime_error("print keyword must be at the start of its statement");
				root = std::make_shared<Node>(NodeType::Print);
				currentNode = std::make_shared<Node>(root);
			}
			else if (currentToken.is(TokenType::Plus))
			{
				if (tokens.currentPosition() == 0)
					throw std::runtime_error("+ is not valid starting token");
				if (!currentNode->right()->is({ NodeType::IntLiteral, NodeType::Variable }))
					throw std::runtime_error("can only add to variables or constants");
				const Token& nextToken = tokens.next();
				if (nextToken.is({ TokenType::IntLiteral, TokenType::Variable }))
				{
					currentNode->setRight(makeAddNode(
						std::move(currentNode->right()),
						std::make_shared<Node>(nextToken.type(), std::stoi(nextToken.lexeme()))
					));
					currentNode = currentNode->right();
				}
				else
					throw std::runtime_error("+ must be followed by an int or variable");
			}
			else if (currentToken.is(TokenType::Minus))
			{
				if (tokens.currentPosition() == 0)
					throw std::runtime_error("- is not valid starting token");
				if (!currentNode->is({ NodeType::IntLiteral, NodeType::Variable }))
					throw std::runtime_error("can only subtract variables or constants");
				const Token& nextToken = tokens.next();
				if (nextToken.is({ TokenType::IntLiteral, TokenType::Variable }))
				{
					currentNode->setRight(makeSubtractNode(
						std::move(currentNode->right()),
						std::make_shared<Node>(nextToken.type(), std::stoi(nextToken.lexeme()))
					));
					currentNode = currentNode->right();
				}
				else
					throw std::runtime_error("- must be followed by an int or variable");
			}
			/*else if (currentToken.is(TokenType::Star))
			{
				const Token& nextToken = tokens.next();
				if (nextToken.is(TokenType::IntLiteral))
				{
					if (root->getPriority() < 2)
					{
						std::unique_ptr<Node> oldRight = std::move(static_cast<BinOpNode*>(root.get())->Right);

						root->setRight(MultiplyNode(
							std::move(oldRight),
							std::make_unique<LeafNode>(std::stoi(nextToken.lexeme()))
						));
					}
					else
					{
						root = MultiplyNode(
							std::move(root),
							std::make_unique<LeafNode>(std::stoi(nextToken.lexeme()))
						);
					}
				}
				else
				{
					throw std::runtime_error("* must be followed by an int");
				}
			}*/
			else if (currentToken.is(TokenType::SemiColon))
			{
				if (tokens.currentPosition() == 0)
					throw std::runtime_error("empty statement");
				if (tokens.hasNext())
					throw std::runtime_error("semi colon must be at the end of the line");
				break;
			}
			else
				throw std::runtime_error("order must be int then operator");

			tokens.next();
		}
		if (!tokens.peekType(0, TokenType::SemiColon))
			throw std::runtime_error("line must end with a semi colon");
		else if (tokens.currentPosition() == 0)
			throw std::runtime_error("empty statement");

		return root;
	}
}