#pragma once

#include "sinful//pch.hpp"

#include "Exceptions.hpp"
#include "Node.hpp"
#include "Token.hpp"

namespace Sinful::Parser
{
	static void expect(Tokens::TokenStream& tokens, Tokens::TokenType type, std::string msg);

	// Level 4: Factors
	static std::unique_ptr<Nodes::Node> parseFactor(Tokens::TokenStream& tokens);

	// Level 3: Terms (Handling * and /)
	static std::unique_ptr<Nodes::Node> parseTerm(Tokens::TokenStream& tokens);

	// Level 2: Expressions (Handling + and -)
	std::unique_ptr<Nodes::Node> parseExpression(Tokens::TokenStream& tokens);

	// Level 1: Statements (Assignment, Print)
	std::unique_ptr <Nodes::Node > parseStatement(Tokens::TokenStream& tokens);
}