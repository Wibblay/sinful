#pragma once

#include <exception>
#include <string>
#include <unordered_map>

class SymbolTable
{
	struct Symbol
	{
		std::string name;
		std::string asmName;
		int stackOffset;
		bool isConstant;
		bool inRegister;
		std::string registerName;
	};

public:

	void addLocalVariable(const std::string& name);
	inline const int getStackOffset(const std::string& name) const;
	const bool contains(std::string& name) const { return _symbols.contains(name); }

private:

	std::unordered_map<std::string, Symbol> _symbols;
	int _currentStackOffset = -8;
};