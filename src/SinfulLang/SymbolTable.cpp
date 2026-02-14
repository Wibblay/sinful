#include "SymbolTable.hpp"

namespace Sinful::Symbols
{
	const Symbol& SymbolTable::addLocalVariable(const std::string& name)
	{
		if (_symbols.contains(name))
			throw std::runtime_error("Variable already defined: " + name);

		Symbol sym{ name, _currentStackOffset };
		_symbols[name] = sym;
		_currentStackOffset -= 8;
		return _symbols.at(name);
	}

	std::optional<Symbol> SymbolTable::getSymbol(const std::string& name) const
	{
		auto it = _symbols.find(name);
		if (it != _symbols.end()) return it->second;
		return std::nullopt;
	}

	int SymbolTable::getStackOffset(const std::string& name) const
	{
		auto sym = getSymbol(name);
		if (!sym)
		{
			throw std::runtime_error("Undefined variable: " + name);
		}
		return sym->stackOffset;
	}
}
