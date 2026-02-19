#pragma once

#include "sinful//pch.hpp"

#include "Exceptions.hpp"
#include "Node.hpp"
#include "Token.hpp"

namespace Sinful::Parser
{
	std::unique_ptr <Nodes::Node> parseProgram(Tokens::TokenStream& tokens);
	std::unique_ptr <Nodes::Node> parseBlock(Tokens::TokenStream& tokens);

	std::unique_ptr <Nodes::Node> parseStatement(Tokens::TokenStream& tokens);
	std::unique_ptr<Nodes::Node> parseExpression(Tokens::TokenStream& tokens);

	static std::unique_ptr<Nodes::Node> parseTerm(Tokens::TokenStream& tokens);
	static std::unique_ptr<Nodes::Node> parseFactor(Tokens::TokenStream& tokens);

	static void expect(Tokens::TokenStream& tokens, Tokens::TokenType type, std::string msg);
}