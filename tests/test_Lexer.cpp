#include <gtest/gtest.h>
#include "Lexer.hpp"

using namespace Sinful::Exceptions;
using namespace Sinful::Lexer;
using namespace Sinful::Tokens;

TEST(LexerTest, TokenisesAssignment)
{
    std::string_view source = "x = 42;";
    Scanner sc{ "Test.sin" };
    sc.newLine(source);
    TokenStream stream{};
    tokeniseLine(sc, stream);

    EXPECT_EQ(stream.size(), 4);
    EXPECT_TRUE(stream.peek(0).is(TokenType::Variable));
    EXPECT_EQ(stream.peek(0).lexeme(), "x");
    EXPECT_TRUE(stream.peek(1).is(TokenType::Equals));
    EXPECT_TRUE(stream.peek(2).is(TokenType::IntLiteral));
    EXPECT_EQ(stream.peek(2).lexeme(), "42");
    EXPECT_TRUE(stream.peek(3).is(TokenType::SemiColon));
}

TEST(LexerTest, TokenisesPrint)
{
    std::string_view source = "print test;";
    Scanner sc{ "Test.sin" };
    sc.newLine(source);
    TokenStream stream{};
    tokeniseLine(sc, stream);

    EXPECT_EQ(stream.size(), 3);
    EXPECT_TRUE(stream.peek(0).is(TokenType::Print));
    EXPECT_TRUE(stream.peek(1).is(TokenType::Variable));
    EXPECT_EQ(stream.peek(1).lexeme(), "test");
    EXPECT_TRUE(stream.peek(2).is(TokenType::SemiColon));
}

TEST(LexerTest, DooesntTokeniseComments)
{
    std::string_view source = "x = 42; // x = 10";
    Scanner sc{ "Test.sin" };
    sc.newLine(source);
    TokenStream stream{};
    tokeniseLine(sc, stream);

    EXPECT_EQ(stream.size(), 4);
    EXPECT_TRUE(stream.peek(0).is(TokenType::Variable));
    EXPECT_EQ(stream.peek(0).lexeme(), "x");
    EXPECT_TRUE(stream.peek(1).is(TokenType::Equals));
    EXPECT_TRUE(stream.peek(2).is(TokenType::IntLiteral));
    EXPECT_EQ(stream.peek(2).lexeme(), "42");
    EXPECT_TRUE(stream.peek(3).is(TokenType::SemiColon));
}

TEST(LexerTest, TokenisesUnderscoreIdentifiers)
{
    std::string_view source = "_my_var = 5;";
    Scanner sc{ "Test.sin" };
    sc.newLine(source);
    TokenStream stream{};
    tokeniseLine(sc, stream);

    EXPECT_EQ(stream.size(), 4);
    EXPECT_TRUE(stream.peek(0).is(TokenType::Variable));
    EXPECT_EQ(stream.peek(0).lexeme(), "_my_var");
}

TEST(LexerTest, ThrowsOnInvalidNumericStart)
{
    std::string_view source = "123var = 5;";
    Scanner sc{ "Test.sin" };
    sc.newLine(source);
    TokenStream stream{};
    EXPECT_THROW(tokeniseLine(sc, stream), CompilerException);
}
