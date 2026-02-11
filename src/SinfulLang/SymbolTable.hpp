#pragma once

#include <exception>
#include <stdexcept>
#include <string>
#include <unordered_map>

class SymbolTable
{
	struct Symbol
	{
		std::string name;
		std::string asmName;
		int stackOffset = 0;
		bool isConstant = false;
		bool inRegister = false;
		std::string registerName;
	};

public:

	SymbolTable() : _symbols() { }

	void addLocalVariable(const std::string& name);
	inline int getStackOffset(const std::string& name) const
	{
		if (!_symbols.contains(name))
			throw std::runtime_error("given name not found in symbol table");

		return _symbols.at(name).stackOffset;
	}
	bool contains(const std::string& name) const { return _symbols.contains(name); }

private:

	std::unordered_map<std::string, Symbol> _symbols;
	int _currentStackOffset = -8;
};