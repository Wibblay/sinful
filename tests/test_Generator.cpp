#include <gtest/gtest.h>
#include "Generator.hpp"

using namespace Sinful::AsmGeneration;
using namespace Sinful::Nodes;
using namespace Sinful::Types;

TEST(GeneratorTest, GeneratesCorrectMathAsm)
{
    Generator gen;
    // Simulate: x = 5 + 10;
    auto left = std::make_unique<Node>(LiteralNode{ "5" });
    auto right = std::make_unique<Node>(LiteralNode{ "10" });
    auto bin = std::make_unique<Node>(BinaryExpr{ "+", std::move(left), std::move(right) });
    auto assign = std::make_unique<Node>(Assignment{ "x", std::move(bin) });

    gen.traverseAsmGenerator(*assign);
    std::string result = gen.generateFinal();

    // Check for key components in the string
    EXPECT_NE(result.find("mov     rax, 5"), std::string::npos);
    EXPECT_NE(result.find("mov     rbx, rax"), std::string::npos);
    EXPECT_NE(result.find("add     rax, rbx"), std::string::npos);
    EXPECT_NE(result.find("[rbp-8], rax"), std::string::npos);
}

TEST(GeneratorTest, GeneratesBooleanLiteralTrue)
{
    Generator gen;
    auto lit = std::make_unique<Node>(LiteralNode{ "true" }, Type::boolean());
    auto assign = std::make_unique<Node>(Assignment{ "flag", std::move(lit) }, Type::boolean());

    gen.traverseAsmGenerator(*assign);
    std::string result = gen.generateFinal();

    EXPECT_NE(result.find("mov     rax, 1"), std::string::npos);
}

TEST(GeneratorTest, GeneratesBooleanLiteralFalse)
{
    Generator gen;
    auto lit = std::make_unique<Node>(LiteralNode{ "false" }, Type::boolean());
    auto assign = std::make_unique<Node>(Assignment{ "flag", std::move(lit) }, Type::boolean());

    gen.traverseAsmGenerator(*assign);
    std::string result = gen.generateFinal();

    EXPECT_NE(result.find("mov     rax, 0"), std::string::npos);
}

TEST(GeneratorTest, GeneratesEqualityComparison)
{
    Generator gen;
    auto left = std::make_unique<Node>(LiteralNode{ "5" }, Type::i32());
    auto right = std::make_unique<Node>(LiteralNode{ "5" }, Type::i32());
    auto bin = std::make_unique<Node>(BinaryExpr{ "==", std::move(left), std::move(right) }, Type::boolean());
    auto assign = std::make_unique<Node>(Assignment{ "result", std::move(bin) }, Type::boolean());

    gen.traverseAsmGenerator(*assign);
    std::string result = gen.generateFinal();

    EXPECT_NE(result.find("cmp"), std::string::npos);
    EXPECT_NE(result.find("sete"), std::string::npos);
    EXPECT_NE(result.find("movzx"), std::string::npos);
}

TEST(GeneratorTest, GeneratesLogicalAnd)
{
    Generator gen;
    auto left = std::make_unique<Node>(LiteralNode{ "1" }, Type::boolean());
    auto right = std::make_unique<Node>(LiteralNode{ "0" }, Type::boolean());
    auto bin = std::make_unique<Node>(BinaryExpr{ "&&", std::move(left), std::move(right) }, Type::boolean());
    auto assign = std::make_unique<Node>(Assignment{ "result", std::move(bin) }, Type::boolean());

    gen.traverseAsmGenerator(*assign);
    std::string result = gen.generateFinal();

    EXPECT_NE(result.find("and     rax, rbx"), std::string::npos);
}

TEST(GeneratorTest, GeneratesLogicalNot)
{
    Generator gen;
    auto operand = std::make_unique<Node>(LiteralNode{ "1" }, Type::boolean());
    auto unary = std::make_unique<Node>(UnaryExpr{ "!", std::move(operand) }, Type::boolean());
    auto assign = std::make_unique<Node>(Assignment{ "result", std::move(unary) }, Type::boolean());

    gen.traverseAsmGenerator(*assign);
    std::string result = gen.generateFinal();

    EXPECT_NE(result.find("test"), std::string::npos);
    EXPECT_NE(result.find("sete"), std::string::npos);
    EXPECT_NE(result.find("movzx"), std::string::npos);
}

TEST(GeneratorTest, GeneratesUnaryMinus)
{
    Generator gen;
    auto operand = std::make_unique<Node>(LiteralNode{ "5" }, Type::i32());
    auto unary = std::make_unique<Node>(UnaryExpr{ "-", std::move(operand) }, Type::i32());
    auto assign = std::make_unique<Node>(Assignment{ "result", std::move(unary) }, Type::i32());

    gen.traverseAsmGenerator(*assign);
    std::string result = gen.generateFinal();

    EXPECT_NE(result.find("neg     rax"), std::string::npos);
}
