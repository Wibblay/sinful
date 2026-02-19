#include "SymbolTable.hpp"

using namespace Sinful::Exceptions;

namespace Sinful::Symbols
{
	SymbolTable::SymbolTable(SymbolTableManager* manager) : _manager(manager), _parent(manager->currentTable) 
	{
		_manager->currentTable = this;
	}

	SymbolTable::~SymbolTable() { _manager->currentTable = _parent; _parent = nullptr; }

	const Symbol& SymbolTable::addLocalVariable(const std::string& name, bool initialised)
	{
		if (containsInCurrentScope(name)) throw CompilerException(Diagnostic{
			Exceptions::Diagnostic::Level::Error,
			{},
			"Variable '" + name + "' is already defined in this scope"
		});

		Symbol sym{ name, _manager->currentStackOffset, initialised };
		_symbols[name] = sym;
		_manager->currentStackOffset -= 8;
		return _symbols.at(name);
	}

	std::optional<Symbol> SymbolTable::getSymbol(const std::string& name) const
	{
		auto it = _symbols.find(name);
		if (it != _symbols.end()) return it->second;
		if (_parent != nullptr) return _parent->getSymbol(name);

		return std::nullopt;
	}

	int SymbolTable::getStackOffset(const std::string& name) const
	{
		auto sym = getSymbol(name);
		if (!sym) throw CompilerException(Diagnostic{
			Exceptions::Diagnostic::Level::Error,
			{},
			"Unrecognised variable '" + name + "'"
		});
		return sym->stackOffset;
	}

	int SymbolTable::getStackOffsetInitialised(const std::string& name) const
	{
		auto sym = getSymbol(name);
		if (!sym) throw CompilerException(Diagnostic{
			Exceptions::Diagnostic::Level::Error,
			{},
			"Unrecognised variable '" + name + "'"
		});
		if (!sym->initialised) throw CompilerException(Diagnostic{
			Exceptions::Diagnostic::Level::Error,
			{},
			"Variable '" + name + "' has not been initialised"
		});
		return sym->stackOffset;
	}

	bool SymbolTable::contains(const std::string& name) const
	{
		if (_symbols.contains(name)) return true;
		if (_parent != nullptr) return _parent->contains(name);
		return false;
	}

	int SymbolTable::getTotalStackSize() const { return std::abs(_manager->currentStackOffset + 8); }
}
