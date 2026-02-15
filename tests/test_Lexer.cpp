#include <gtest/gtest.h>
#include "Lexer.hpp"

using namespace Sinful::Lexer;
using namespace Sinful::Tokens;

TEST(LexerTest, TokenisesAssignment)
{
    std::string_view source = "x = 42;";
    auto stream = tokeniseLine(source, "Test.sin");

    EXPECT_EQ(stream->size(), 4);
    EXPECT_TRUE(stream->peek(0).is(TokenType::Variable));
    EXPECT_EQ(stream->peek(0).lexeme(), "x");
    EXPECT_TRUE(stream->peek(1).is(TokenType::Equals));
    EXPECT_TRUE(stream->peek(2).is(TokenType::IntLiteral));
    EXPECT_EQ(stream->peek(2).lexeme(), "42");
    EXPECT_TRUE(stream->peek(3).is(TokenType::SemiColon));
}

TEST(LexerTest, ThrowsOnInvalidNumericStart)
{
    std::string_view source = "123var = 5;";
    EXPECT_THROW(tokeniseLine(source, "Test.sin"), std::runtime_error);
}
