#pragma once

#include "sinful/pch.hpp"

#include "Exceptions.hpp"
#include "Types.hpp"

namespace Sinful::Symbols
{
	struct Symbol
	{
		std::string name;
		Types::Type type;
		int stackOffset = 0;
		bool initialised = false;
	};

	struct SymbolTableManager;

	class SymbolTable
	{
	public:

		explicit SymbolTable(SymbolTableManager* manager);
		~SymbolTable();

		const Symbol& addLocalVariable(const std::string& name, Types::Type type, bool initialised = false);
		const Symbol& addLocalVariable(const std::string& name) { return addLocalVariable(name, Types::Type::unresolved()); }
		std::optional<Symbol> getSymbol(const std::string& name) const;
		int getStackOffset(const std::string& name) const;
		int getStackOffsetIfInitialised(const std::string& name) const;

		bool containsInCurrentScope(const std::string& name) const { return _symbols.contains(name); }
		bool contains(const std::string& name) const;
		void ensureInitialised(const std::string& name);
		int getTotalStackSize() const;

	private:

		SymbolTableManager* _manager;
		SymbolTable* _parent;
		std::unordered_map<std::string, Symbol> _symbols;
	};

	struct SymbolTableManager
	{
		SymbolTableManager() { globalTable = std::make_unique<SymbolTable>(this); }
		~SymbolTableManager() { while (currentTable != globalTable.get()) { delete currentTable; } }

		int getTotalStackSize() const { return std::abs(currentStackOffset + 8); }

		std::unique_ptr<SymbolTable> globalTable;
		SymbolTable* currentTable = nullptr;
		int currentStackOffset = -8;
	};		
}
