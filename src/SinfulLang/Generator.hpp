#pragma once

#include <fstream>
#include <sstream>
#include <vector>
#include <iomanip>

#include "Node.hpp"
#include "SymbolTable.hpp"
#include "Token.hpp"

class Generator
{
public:

    Generator() : _symbolTable(), _dataSection(), _codeSection() 
    { 
        setupStackFrame();
    }
    ~Generator() = default;

    std::string generate();
    void asmFromStatement(std::shared_ptr<Node> root);  

private:
    // -- Low-level Emitters --
    void emit(const std::string& instr, const std::string& ops = "");
    void push(const std::string& reg);
    void pop(const std::string& reg);
    void mov(const std::string& dest, const std::string& src);
    void add(const std::string& dest, const std::string& src);
    void sub(const std::string& dest, const std::string& src);
    void set0(const std::string& reg);


    // -- High-level Helpers --
    void setupStackFrame();
    void tearDownStackFrame();
    void generateGlobalData();
    void generateExpr(const std::shared_ptr<Node> node);
    void generateExprOperand(const std::shared_ptr<Node> node);
    void generateAssignment(const std::shared_ptr<Node> node);
    void generatePrint(const std::shared_ptr<Node> node);
    void intToString();
    void getStdOutHandle();

    SymbolTable _symbolTable;
    std::stringstream _dataSection;
    std::stringstream _codeSection;
    std::stringstream _stackVars;
    int _labelCounter = 0;
    int _stringLiteralCounter = 0;
    bool _hasStdOutHandle = false;
};