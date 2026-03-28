#include "Parser.hpp"

using namespace Sinful::Exceptions;
using namespace Sinful::Nodes;
using namespace Sinful::Tokens;
using namespace Sinful::Types;

namespace Sinful::Parser
{
	static std::unique_ptr<Node> parseAddSub(TokenStream& tokens);
	static std::unique_ptr<Node> parseTerm(TokenStream& tokens);
	static std::unique_ptr<Node> parseUnary(TokenStream& tokens);
	static std::unique_ptr<Node> parseFactor(TokenStream& tokens);

	std::unique_ptr<Node> parseProgram(TokenStream& tokens)
	{
		auto loc = tokens.peek().location();
		std::vector<std::unique_ptr<Node>> program{};
		while (tokens.hasNext())
			program.emplace_back(parseStatement(tokens));

		return std::make_unique<Node>(ScopeNode{ std::move(program) }, Type::none(), loc);
	}

	std::unique_ptr<Node> parseBlock(TokenStream& tokens)
	{
		auto loc = tokens.peek().location();
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
		return std::make_unique<Node>(ScopeNode{ std::move(scopedStatements) }, Type::none(), loc);
	}

	std::unique_ptr<Node> parseStatement(TokenStream& tokens)
	{
		if (tokens.peek().is(TokenType::LBrace))
			return parseBlock(tokens);

		if (tokens.peek().is(TokenType::Print))
		{
			auto loc = tokens.peek().location();
			tokens.next();
			auto expr = parseExpression(tokens);
			expectType(*expr, Type::i32(), "May only print numeric values");
			expect(tokens, TokenType::SemiColon, "Missing semicolon after print");
			return std::make_unique<Node>(PrintStmt{ std::move(expr) }, Type::i32(), loc);
		}

		// Assignment: Variable = [Type] Expr ;   or   Variable = Type ;  (default init)
		if (tokens.peek().is(TokenType::Variable) && tokens.peek(1).is(TokenType::Equals))
		{
			std::string name = tokens.peek().lexeme();
			auto loc = tokens.peek().location();
			tokens.next(); // var
			tokens.next(); // =

			Type enforcedType = Type::none();
			std::unique_ptr<Node> expr;

			if (tokens.peek().is({ TokenType::I32Type, TokenType::BoolType }))
			{
				enforcedType = convertTypeToken(tokens.peek().type());
				if (tokens.peek(1).is(TokenType::SemiColon))
				{
					// foo = i32; — default-initialise to type's zero value
					expr = defaultFactorForType(tokens); // peeks type token before consuming
					tokens.next(); // consume type token
				}
				else
				{
					tokens.next(); // consume type token
					expr = parseExpression(tokens);
					expectType(*expr, enforcedType, "Expression did not match stated type");
				}
			}
			else
			{
				expr = parseExpression(tokens);
			}

			expect(tokens, Tokens::TokenType::SemiColon, "Missing semicolon after assignment");
			Type nodeType = enforcedType.isNone() ? expr->type : enforcedType;
			return std::make_unique<Node>(Assignment{ name, std::move(expr), false, loc }, nodeType, loc);
		}

		throw CompilerException(Diagnostic{
			Exceptions::Diagnostic::Level::Error,
			tokens.peek().location(),
			"Statement does not conform to recognised pattern"
		});
	}

	static std::unique_ptr<Node> parseCondition(TokenStream& tokens)
	{
		auto left = parseAddSub(tokens);
		if (tokens.peek().is({ TokenType::DubEquals, TokenType::ExclEquals, TokenType::LessThan, TokenType::LeOrEqual,
			TokenType::GreaterThan, TokenType::GrOrEqual }))
		{
			auto loc = left->location;
			std::string op = tokens.peek().lexeme();
			tokens.next();
			auto right = parseAddSub(tokens);
			resolveNodeTypes(*left, *right);
			left = std::make_unique<Node>(BinaryExpr{ op, std::move(left), std::move(right) },
				Type::boolean(), loc);
		}
		return left;
	}

	static std::unique_ptr<Node> parseLogicalAnd(TokenStream& tokens)
	{
		auto left = parseCondition(tokens);
		while (tokens.peek().is(TokenType::AmpAmp))
		{
			auto loc = left->location;
			tokens.next();
			auto right = parseCondition(tokens);
			expectType(*left, Type::boolean(), "&& operator requires boolean operands");
			expectType(*right, Type::boolean(), "&& operator requires boolean operands");
			left = std::make_unique<Node>(BinaryExpr{ "&&", std::move(left), std::move(right) },
				Type::boolean(), loc);
		}
		return left;
	}

	static std::unique_ptr<Node> parseLogicalOr(TokenStream& tokens)
	{
		auto left = parseLogicalAnd(tokens);
		while (tokens.peek().is(TokenType::PipePipe))
		{
			auto loc = left->location;
			tokens.next();
			auto right = parseLogicalAnd(tokens);
			expectType(*left, Type::boolean(), "|| operator requires boolean operands");
			expectType(*right, Type::boolean(), "|| operator requires boolean operands");
			left = std::make_unique<Node>(BinaryExpr{ "||", std::move(left), std::move(right) },
				Type::boolean(), loc);
		}
		return left;
	}

	std::unique_ptr<Node> parseExpression(TokenStream& tokens)
	{
		return parseLogicalOr(tokens);
	}

	static std::unique_ptr<Node> parseAddSub(TokenStream& tokens)
	{
		auto left = parseTerm(tokens);
		while (tokens.peek().is({ TokenType::Plus, TokenType::Minus }))
		{
			std::string op = tokens.peek().lexeme();
			tokens.next();
			auto right = parseTerm(tokens);
			resolveNodeTypes(*left, *right);
			expectType(*left, Type::i32(), "Arithmetic operators require i32 operands");
			left = std::make_unique<Node>(BinaryExpr{ op, std::move(left), std::move(right) },
				left->type, left->location);
		}
		return left;
	}

	static std::unique_ptr<Node> parseTerm(TokenStream& tokens)
	{
		auto left = parseUnary(tokens);
		while (tokens.peek().is({ TokenType::Star, TokenType::FSlash }))
		{
			std::string op = tokens.peek().lexeme();
			tokens.next();
			auto right = parseUnary(tokens);
			resolveNodeTypes(*left, *right);
			expectType(*left, Type::i32(), "Arithmetic operators require i32 operands");
			left = std::make_unique<Node>(BinaryExpr{ op, std::move(left), std::move(right) }, left->type, left->location);
		}
		return left;
	}

	static std::unique_ptr<Node> parseUnary(TokenStream& tokens)
	{
		if (tokens.peek().is(TokenType::Exclamation))
		{
			auto loc = tokens.peek().location();
			tokens.next();
			auto operand = parseUnary(tokens);
			expectType(*operand, Type::boolean(), "! operator requires boolean operand");
			return std::make_unique<Node>(UnaryExpr{ "!", std::move(operand) }, Type::boolean(), loc);
		}
		if (tokens.peek().is(TokenType::Minus))
		{
			auto loc = tokens.peek().location();
			tokens.next();
			auto operand = parseUnary(tokens);
			expectType(*operand, Type::i32(), "Unary - operator requires i32 operand");
			return std::make_unique<Node>(UnaryExpr{ "-", std::move(operand) }, Type::i32(), loc);
		}
		return parseFactor(tokens);
	}

	static std::unique_ptr<Node> parseFactor(TokenStream& tokens)
	{
		auto& t = tokens.peek();
		if (t.is(TokenType::IntLiteral))
		{
			tokens.next();
			return std::make_unique<Node>(LiteralNode{ t.lexeme() }, Type::i32(), t.location());
		}
		if (t.is(TokenType::True))
		{
			tokens.next();
			return std::make_unique<Node>(LiteralNode{ "true" }, Type::boolean(), t.location());
		}
		if (t.is(TokenType::False))
		{
			tokens.next();
			return std::make_unique<Node>(LiteralNode{ "false" }, Type::boolean(), t.location());
		}
		if (t.is(TokenType::Variable))
		{
			std::string name = t.lexeme();
			tokens.next();
			return std::make_unique<Node>(VariableNode{ name }, Type::unresolved(), t.location());
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

	static std::unique_ptr<Node> defaultFactorForType(TokenStream& tokens)
	{
		switch (tokens.peek().type())
		{
			using enum TokenType;
		case I32Type:	return std::make_unique<Node>(LiteralNode{ "0" });
		case BoolType:  return std::make_unique<Node>(LiteralNode{ "false" });
		default:
			throw CompilerException(Diagnostic{
				Exceptions::Diagnostic::Level::Error,
				tokens.peek().location(),
				"Expected type token but received '" + tokenTypeToString(tokens.peek().type()) + "'"
			});
		}
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

	static void resolveNodeTypes(Node& left, Node& right)
	{
		if (left.type.isUnresolved())
		{
			if (!right.type.isUnresolved()) left.type = right.type;
		}
		else if (right.type.isUnresolved())
			right.type = left.type;
		else
			expectType(right, left.type, "Expression operand types did not match");
	}

	static void expectType(const Nodes::Node& node, Types::Type type, std::string msg)
	{
		if (node.type.isUnresolved()) return; // resolved at codegen time
		if (node.type != type)
		{
			throw CompilerException(Diagnostic{
				Exceptions::Diagnostic::Level::Error,
				node.location,
				msg + "\nExpected '" + dataTypeToString(type) + "' but found '" + dataTypeToString(node.type) + "'"
			});
		}
	}
}
