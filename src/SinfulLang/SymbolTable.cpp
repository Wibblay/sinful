#include "SymbolTable.hpp"

void SymbolTable::addLocalVariable(const std::string& name)
{
	Symbol sym;
	sym.name = name;
	sym.asmName = name;
	sym.stackOffset = _currentStackOffset;
	_symbols[name] = sym;
	_currentStackOffset -= 8;
}
