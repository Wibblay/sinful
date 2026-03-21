#include <gtest/gtest.h>
#include "Generator.hpp"

using namespace Sinful::AsmGeneration;
using namespace Sinful::Nodes;

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
