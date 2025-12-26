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

inline const int SymbolTable::getStackOffset(const std::string& name) const
{
	if (!_symbols.contains(name))
		throw std::runtime_error("given name not found in symbol table")

		return _symbols[name].stackOffset;
}
