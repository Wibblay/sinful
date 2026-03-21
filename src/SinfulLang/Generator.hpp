#pragma once

#include "sinful/pch.hpp"

#include "Exceptions.hpp"
#include "Node.hpp"
#include "SymbolTable.hpp"
#include "Token.hpp"

namespace Sinful::AsmGeneration
{
    template<class... Ts> struct Overloaded : Ts... { using Ts::operator()...; };
    template<class... Ts> Overloaded(Ts...) -> Overloaded<Ts...>;

    class Generator
    {
    public:

        Generator() : _symbolTable(), _dataSection(), _codeSection() {}
        ~Generator() = default;

        void traverseAsmGenerator(const Nodes::Node& node);
        std::string generateFinal();

        // -- Low-level Emitters --
        void emit(const std::string& instr, const std::string& ops, std::stringstream& target);
        void emit(const std::string& instr, const std::string& ops = "") { emit(instr, ops, _codeSection); }

        void push(const std::string& reg, std::stringstream& target);
        void push(const std::string& reg) { push(reg, _codeSection); }

        void pop(const std::string& reg, std::stringstream& target);
        void pop(const std::string& reg) { pop(reg, _codeSection); }

        void mov(const std::string& dest, const std::string& src, std::stringstream& target);
        void mov(const std::string& dest, const std::string& src) { mov(dest, src, _codeSection); }

        void add(const std::string& dest, const std::string& src, std::stringstream& target);
        void add(const std::string& dest, const std::string& src) { add(dest, src, _codeSection); }

        void sub(const std::string& dest, const std::string& src, std::stringstream& target);
        void sub(const std::string& dest, const std::string& src) { sub(dest, src, _codeSection); }

        void set0(const std::string& reg, std::stringstream& target);
        void set0(const std::string& reg) { set0(reg, _codeSection); }

    private:

        // -- High-level Helpers --
        void setupStackFrame(std::stringstream& target);
        void setupStackFrame() { setupStackFrame(_codeSection); }

        void tearDownStackFrame(std::stringstream& target);
        void tearDownStackFrame() { tearDownStackFrame(_codeSection); }

        void generateGlobalData();
        void intToString();
        void getStdOutHandle(std::stringstream& target);

        Symbols::SymbolTableManager _symbolTable;
        std::stringstream _dataSection;
        std::stringstream _codeSection;
        std::stringstream _stackVars;
        int _labelCounter = 0;
        int _stringLiteralCounter = 0;
        bool _requiresPrint = false;
        int _lastLocalSpaceReserved = 0;
    };
}
