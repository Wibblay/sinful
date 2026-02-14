#pragma once

#include <exception>
#include <optional>
#include <stdexcept>
#include <string>
#include <unordered_map>

namespace Sinful::Symbols
{
	struct Symbol
	{
		std::string name;
		int stackOffset = 0;
	};

	class SymbolTable
	{
	public:

		SymbolTable() = default;

		const Symbol& addLocalVariable(const std::string& name);
		std::optional<Symbol> getSymbol(const std::string& name) const;
		int getStackOffset(const std::string& name) const;

		bool contains(const std::string& name) const { return _symbols.contains(name); }
		int getTotalStackSize() const { return std::abs(_currentStackOffset + 8); }

	private:

		std::unordered_map<std::string, Symbol> _symbols;
		int _currentStackOffset = -8;
	};
}
