#include <gtest/gtest.h>
#include "Parser.hpp"

using namespace Sinful::Tokens;
using namespace Sinful::Nodes;
using namespace Sinful::Parser;
using namespace Sinful::Exceptions;

static SourceLocation defaultLoc{ "", 0, 0 };
static SourceLocation testLoc{ "", 1, 4 };

TEST(ParserTest, RespectsOperatorPrecedence)
{
    // 5 + 2 * 3;
    std::vector<Token> tokens = {
        {TokenType::IntLiteral, "5", defaultLoc},
        {TokenType::Plus, "+", defaultLoc},
        {TokenType::IntLiteral, "2", defaultLoc},
        {TokenType::Star, "*", defaultLoc},
        {TokenType::IntLiteral, "3", defaultLoc},
        {TokenType::SemiColon, ";", defaultLoc}
    };
    TokenStream stream(tokens);
    auto root = parseExpression(stream);

    // Root should be "+" (the lowest precedence handled last)
    ASSERT_TRUE(std::holds_alternative<BinaryExpr>(root->data));
    auto& binExpr = std::get<BinaryExpr>(root->data);
    EXPECT_EQ(binExpr.op, "+");

    // Right child should be "*"
    ASSERT_TRUE(std::holds_alternative<BinaryExpr>(binExpr.right->data));
    auto& rightSide = std::get<BinaryExpr>(binExpr.right->data);
    EXPECT_EQ(rightSide.op, "*");
}

TEST(ParserTest, ParsesUntypedAssignment)
{
    std::vector<Token> tokens = {
        {TokenType::Variable, "x", testLoc},
        {TokenType::Equals, "=", defaultLoc},
        {TokenType::IntLiteral, "5", defaultLoc},
        {TokenType::SemiColon, ";", defaultLoc}
    };
    TokenStream stream(tokens);
    auto root = parseStatement(stream);

    // Root should be assignment
    ASSERT_TRUE(std::holds_alternative<Assignment>(root->data));
    auto& assign = std::get<Assignment>(root->data);
    EXPECT_EQ(assign.name, "x");
    EXPECT_EQ(assign.mustDeclare, false);
    EXPECT_EQ(assign.location, testLoc);

    // Right child should be literal 5
    ASSERT_TRUE(std::holds_alternative<LiteralNode>(assign.value->data));
    auto& literal = std::get<LiteralNode>(assign.value->data);
    EXPECT_EQ(literal.value, "5");
}

TEST(ParserTest, RejectsTypedAssignment)
{
    // i32 x = 5; is not valid grammar — type prefixes on assignments are not accepted
    std::vector<Token> tokens = {
        {TokenType::I32Type, defaultLoc},
        {TokenType::Variable, "x", testLoc},
        {TokenType::Equals, "=", defaultLoc},
        {TokenType::IntLiteral, "5", defaultLoc},
        {TokenType::SemiColon, ";", defaultLoc}
    };
    TokenStream stream(tokens);
    EXPECT_THROW(parseStatement(stream), CompilerException);
}

TEST(ParserTest, RejectsDeclaration)
{
    // i32 x; is not valid grammar — type-prefixed declarations are not accepted
    std::vector<Token> tokens = {
        {TokenType::I32Type, defaultLoc},
        {TokenType::Variable, "x", testLoc},
        {TokenType::SemiColon, ";", defaultLoc}
    };
    TokenStream stream(tokens);
    EXPECT_THROW(parseStatement(stream), CompilerException);
}

TEST(ParserTest, ParsesPrint)
{
    SourceLocation defaultLoc{ "", 0, 0 };

    std::vector<Token> tokens = {
        {TokenType::Print, defaultLoc},
        {TokenType::IntLiteral, "5", defaultLoc},
        {TokenType::Plus, "+", defaultLoc},
        {TokenType::IntLiteral, "2", defaultLoc},
        {TokenType::Star, "*", defaultLoc},
        {TokenType::IntLiteral, "3", defaultLoc},
        {TokenType::SemiColon, ";", defaultLoc}
    };
    TokenStream stream(tokens);
    auto root = parseStatement(stream);

    // Root should be print
    ASSERT_TRUE(std::holds_alternative<PrintStmt>(root->data));
    auto& prnt = std::get<PrintStmt>(root->data);

    // Full expression should be printed
    ASSERT_TRUE(std::holds_alternative<BinaryExpr>(prnt.value->data));
}

TEST(ParserTest, ParsesScope)
{
    SourceLocation defaultLoc{ "", 0, 0 };

    std::vector<Token> tokens = {
        {TokenType::LBrace, "{", defaultLoc},
        {TokenType::Variable, "x", defaultLoc},
        {TokenType::Equals, "=", defaultLoc},
        {TokenType::IntLiteral, "5", defaultLoc},
        {TokenType::SemiColon, ";", defaultLoc},
        {TokenType::Print, defaultLoc},
        {TokenType::Variable, "x", defaultLoc},
        {TokenType::SemiColon, ";", defaultLoc},
        {TokenType::RBrace, "}", defaultLoc}
    };
    TokenStream stream(tokens);
    auto root = parseStatement(stream);

    // Root should be scope
    ASSERT_TRUE(std::holds_alternative<ScopeNode>(root->data));
    auto& scope = std::get<ScopeNode>(root->data);
    EXPECT_EQ(scope.statements.size(), size_t(2));

    // Should contain assignment and print
    ASSERT_TRUE(std::holds_alternative<Assignment>(scope.statements[0]->data));
    ASSERT_TRUE(std::holds_alternative<PrintStmt>(scope.statements[1]->data));
}

TEST(ParserTest, ThrowsOnUnclosedScope)
{
    SourceLocation defaultLoc{ "", 0, 0 };

    std::vector<Token> tokens = {
        {TokenType::LBrace, "{", defaultLoc},
        {TokenType::Variable, "x", defaultLoc},
        {TokenType::Equals, "=", defaultLoc},
        {TokenType::IntLiteral, "5", defaultLoc},
        {TokenType::SemiColon, ";", defaultLoc},
        {TokenType::Print, defaultLoc},
        {TokenType::Variable, "x", defaultLoc},
        {TokenType::SemiColon, ";", defaultLoc}
    };
    TokenStream stream(tokens);
    EXPECT_THROW(parseStatement(stream), CompilerException);
}
