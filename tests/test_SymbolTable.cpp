#include <gtest/gtest.h>
#include "SymbolTable.hpp"

using namespace Sinful::Symbols;

TEST(SymbolTableTest, ManagesOffsets)
{
    SymbolTable table;

    auto& sym1 = table.addLocalVariable("a");
    EXPECT_EQ(sym1.stackOffset, -8);

    auto& sym2 = table.addLocalVariable("b");
    EXPECT_EQ(sym2.stackOffset, -16);

    EXPECT_EQ(table.getTotalStackSize(), 16);
}

TEST(SymbolTableTest, DetectsRedefinition)
{
    SymbolTable table;
    table.addLocalVariable("x");
    EXPECT_THROW(table.addLocalVariable("x"), std::runtime_error);
}
