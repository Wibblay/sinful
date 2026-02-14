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

namespace Sinful::Parser
{
	static void expect(Tokens::TokenStream& tokens, Tokens::TokenType type, std::string_view msg)
	{
		if (!tokens.peek().is(type)) throw std::runtime_error(msg.data());
		tokens.next();
	}

	// Level 4: Factors
	static std::unique_ptr<Nodes::Node> parseFactor(Tokens::TokenStream& tokens);

	// Level 3: Terms (Handling * and /)
	static std::unique_ptr<Nodes::Node> parseTerm(Tokens::TokenStream& tokens);

	// Level 2: Expressions (Handling + and -)
	std::unique_ptr<Nodes::Node> parseExpression(Tokens::TokenStream& tokens);

	// Level 1: Statements (Assignment, Print)
	std::unique_ptr <Nodes::Node > parseStatement(Tokens::TokenStream& tokens);
}