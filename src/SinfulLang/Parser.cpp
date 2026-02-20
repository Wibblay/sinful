#include "Parser.hpp"

using namespace Sinful::Nodes;
using namespace Sinful::Tokens;
using namespace Sinful::Exceptions;

namespace Sinful::Parser
{
	std::unique_ptr<Node> parseProgram(TokenStream& tokens)
	{
		std::vector<std::unique_ptr<Node>> program{};
		while (tokens.hasNext())
			program.emplace_back(parseStatement(tokens));

		return std::make_unique<Node>(ScopeNode{ std::move(program) });
	}

	std::unique_ptr<Node> parseBlock(TokenStream& tokens)
	{
		std::vector<std::unique_ptr<Node>> nodes{};
		while (!tokens.peek().is(TokenType::RBrace))
			nodes.emplace_back(parseStatement(tokens));
		expect(tokens, TokenType::RBrace, "Unclosed scope");
		return std::make_unique<Node>(ScopeNode{ std::move(nodes) });
	}

	std::unique_ptr<Node> parseStatement(TokenStream& tokens)
	{
		if (tokens.peek().is(TokenType::LBrace))
		{
			tokens.next();
			std::vector<std::unique_ptr<Node>> scopedStatements;
			while (!tokens.peek().is(TokenType::RBrace))
			{
				auto stmt = parseStatement(tokens);
				scopedStatements.emplace_back(std::move(stmt));
				if (!tokens.hasNext())
					break;
			}
			expect(tokens, TokenType::RBrace, "Unclosed scope");
			return std::make_unique<Node>(ScopeNode{ std::move(scopedStatements) });
		}

		if (tokens.peek().is(TokenType::Print))
		{
			tokens.next();
			auto expr = parseExpression(tokens);
			expect(tokens, TokenType::SemiColon, "Missing semicolon after print");
			return std::make_unique<Node>(PrintStmt{ std::move(expr) });
		}

		if (tokens.peek().is(TokenType::I32Type))
		{
			tokens.next();
			if (tokens.peek().is(TokenType::Variable) && tokens.peek(1).is(TokenType::Equals))
			{
				std::string name = tokens.peek().lexeme();
				tokens.next(); // var
				tokens.next(); // =
				auto expr = parseExpression(tokens);
				expect(tokens, Tokens::TokenType::SemiColon, "Missing semicolon after assignment");
				return std::make_unique<Node>(Assignment{ name, std::move(expr) });
			}
			else if (tokens.peek().is(TokenType::Variable))
			{
				std::string name = tokens.peek().lexeme();
				SourceLocation loc = tokens.peek().location();
				tokens.next();
				expect(tokens, TokenType::SemiColon, "Missing semicolon after declaration");
				return std::make_unique<Node>(Declaration{ name, loc });
			}
		}

		if (tokens.peek().is(TokenType::Variable) && tokens.peek(1).is(TokenType::Equals))
		{
			std::string name = tokens.peek().lexeme();
			tokens.next(); // var
			tokens.next(); // =
			auto expr = parseExpression(tokens);
			expect(tokens, Tokens::TokenType::SemiColon, "Missing semicolon after assignment");
			return std::make_unique<Node>(Assignment{ name, std::move(expr) });
		}

		throw CompilerException(Diagnostic{
			Exceptions::Diagnostic::Level::Error,
			tokens.peek().location(),
			"Statement does not conform to recognised pattern"
		});
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

	static std::unique_ptr<Node> parseTerm(TokenStream& tokens)
	{
		auto left = parseFactor(tokens);
		while (tokens.peek().is({ TokenType::Star, TokenType::FSlash }))
		{
			std::string op = tokens.peek().lexeme();
			tokens.next();
			auto right = parseFactor(tokens);
			left = std::make_unique<Node>(BinaryExpr{ op, std::move(left), std::move(right) });
		}
		return left;
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
			return std::make_unique<Node>(VariableNode{ name, t.location() });
		}
		if (t.is(TokenType::LBracket))
		{
			tokens.next();
			auto bracketExpr = parseExpression(tokens);
			auto& t2 = tokens.peek();
			if (!t2.is(TokenType::RBracket))
				throw CompilerException(Diagnostic{
					Exceptions::Diagnostic::Level::Error,
					t2.location(),
					"Unclosed parentheses"
					});
			tokens.next();
			return bracketExpr;
		}
		throw CompilerException(Diagnostic{
				Exceptions::Diagnostic::Level::Error,
				t.location(),
				"Expected literal or variable but found '" + t.lexeme() + "'"
			});
	}

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
}