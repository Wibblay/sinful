#include <gtest/gtest.h>
#include "Parser.hpp"

using namespace Sinful::Tokens;
using namespace Sinful::Nodes;
using namespace Sinful::Parser;
using namespace Sinful::Exceptions;

TEST(ParserTest, RespectsPrecedence)
{
    SourceLocation defaultLoc{ "", 0, 0 };

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