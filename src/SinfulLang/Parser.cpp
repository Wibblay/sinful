#include "Parser.hpp"

using namespace Sinful::Nodes;
using namespace Sinful::Tokens;
using namespace Sinful::Exceptions;

namespace Sinful::Parser
{
	static void expect(Tokens::TokenStream& tokens, Tokens::TokenType type, std::string msg)
	{
		if (!tokens.peek().is(type))
		{
			auto& token = tokens.peek();
			throw CompilerException(Diagnostic{
				Exceptions::Diagnostic::Level::Error,
				token.location(),
				msg + "\nExpected '" + tokenTypeToString(type) + "' but found '" + token.lexeme() + "'"
			});
		}
		tokens.next();
	}

	static std::unique_ptr<Node> parseFactor(TokenStream& tokens)
	{
		auto& t = tokens.peek();
		if (t.is(TokenType::IntLiteral))
		{
			int val = std::stoi(t.lexeme());
			tokens.next();
			return std::make_unique<Node>(LiteralNode{ val });
		}
		if (t.is(TokenType::Variable))
		{
			std::string name = t.lexeme();
			tokens.next();
			return std::make_unique<Node>(VariableNode{ name });
		}
		throw CompilerException(Diagnostic{
				Exceptions::Diagnostic::Level::Error,
				t.location(),
				"Expected literal or variable but found '" + t.lexeme() + "'"
		});
	}

	static std::unique_ptr<Node> parseTerm(TokenStream& tokens)
	{
		auto left = parseFactor(tokens);
		while (tokens.peek().is(TokenType::Star))
		{
			tokens.next();
			auto right = parseFactor(tokens);
			left = std::make_unique<Node>(BinaryExpr{ "*", std::move(left), std::move(right) });
		}
		return left;
	}

	std::unique_ptr<Node> parseExpression(TokenStream& tokens)
	{
		auto left = parseTerm(tokens);
		while (tokens.peek().is({ TokenType::Plus, TokenType::Minus }))
		{
			std::string op = tokens.peek().lexeme();
			tokens.next();
			auto right = parseTerm(tokens);
			left = std::make_unique<Node>(BinaryExpr{ op, std::move(left), std::move(right) });
		}
		return left;
	}

	std::unique_ptr <Nodes::Node > parseStatement(Tokens::TokenStream& tokens)
	{
		if (tokens.peek().is(Tokens::TokenType::Print))
		{
			tokens.next();
			auto expr = parseExpression(tokens);
			expect(tokens, Tokens::TokenType::SemiColon, "Missing semicolon after print");
			return std::make_unique<Nodes::Node>(Nodes::PrintStmt{ std::move(expr) });
		}

		if (tokens.peek().is(Tokens::TokenType::Variable) && tokens.peek(1).is(Tokens::TokenType::Equals))
		{
			std::string name = tokens.peek().lexeme();
			tokens.next(); // var
			tokens.next(); // =
			auto expr = parseExpression(tokens);
			expect(tokens, Tokens::TokenType::SemiColon, "Missing semicolon after assignment");
			return std::make_unique<Nodes::Node>(Nodes::Assignment{ name, std::move(expr) });
		}

		throw std::runtime_error("Invalid statement");
	}
}