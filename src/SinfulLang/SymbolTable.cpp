#include "SymbolTable.hpp"

using namespace Sinful::Exceptions;
using namespace Sinful::Types;

namespace Sinful::Symbols
{
	SymbolTable::SymbolTable(SymbolTableManager* manager)
		: _manager(manager), _parent(manager->currentTable),
		  _savedStackOffset(manager->currentStackOffset)
	{
		_manager->currentTable = this;
	}

	SymbolTable::~SymbolTable()
	{
		if (_parent != nullptr)
			_manager->currentStackOffset = _savedStackOffset;
		_manager->currentTable = _parent;
		_parent = nullptr;
	}

	const Symbol& SymbolTable::addLocalVariable(const std::string& name, Type type, bool initialised)
	{
		if (containsInCurrentScope(name)) throw CompilerException(Diagnostic{
			Exceptions::Diagnostic::Level::Error,
			{},
			"Variable '" + name + "' is already defined in this scope"
		});

		Symbol sym{ name, type, _manager->currentStackOffset, initialised };
		_symbols[name] = sym;
		int size = getDataTypeSize(type);
		int alignedSize = (size + 7) & ~7;
		if (alignedSize == 0) alignedSize = 8;
		_manager->currentStackOffset -= alignedSize;
		if (_manager->currentStackOffset < _manager->peakStackOffset)
			_manager->peakStackOffset = _manager->currentStackOffset;
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

	int SymbolTable::getStackOffsetIfInitialised(const std::string& name) const
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

	void SymbolTable::ensureInitialised(const std::string& name)
	{
		auto it = _symbols.find(name);
		if (it != _symbols.end())
		{
			it->second.initialised = true; 
			return;
		}
		if (_parent == nullptr) throw CompilerException(Diagnostic{
			Exceptions::Diagnostic::Level::Error,
			{},
			"Unrecognised variable '" + name + "'"
		});
		_parent->ensureInitialised(name);
	}

	int SymbolTable::getTotalStackSize() const { return std::abs(_manager->peakStackOffset + 8); }
}
