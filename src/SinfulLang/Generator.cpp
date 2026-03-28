#include "Generator.hpp"

using namespace Sinful::Exceptions;
using namespace Sinful::Nodes;
using namespace Sinful::Symbols;

namespace Sinful::AsmGeneration
{
    void Generator::traverseAsmGenerator(const Node& node)
    {
        std::visit(Overloaded{
            [&](const LiteralNode& n)
            {
                if (n.value == "true") { mov("rax", "1"); return; }
                if (n.value == "false") { mov("rax", "0"); return; }
                mov("rax", n.value);
            },
            [&](const VariableNode& n)
            {
                try
                {
                    auto offset = _symbolTable.currentTable->getStackOffsetIfInitialised(n.name);
                    mov("rax", "[rbp" + std::to_string(offset) + "]");
                }
                catch (CompilerException e)
                {
                    e.setLocation(node.location);
                    throw e;
                }
            },
            [&](const BinaryExpr& n)
            {
                traverseAsmGenerator(*n.left); // Evaluate left into rax and push
                push("rax");

                traverseAsmGenerator(*n.right); // Evaluate right into rax and move to rbx
                mov("rbx", "rax");

                pop("rax"); // Pop left and operate (result in rax)
                if (n.op == "+") add("rax", "rbx");
                else if (n.op == "-") sub("rax", "rbx");
                else if (n.op == "*") emit("imul", "rax, rbx");
                else if (n.op == "/")
                {
                    set0("rdx");
                    emit("idiv", "rbx");
                }
                else if (n.op == "==" || n.op == "!=" || n.op == "<" || n.op == "<=" || n.op == ">" || n.op == ">=")
                {
                    emit("cmp", "rax, rbx");
                    if (n.op == "==")      emit("sete", "al");
                    else if (n.op == "!=") emit("setne", "al");
                    else if (n.op == "<")  emit("setl", "al");
                    else if (n.op == "<=") emit("setle", "al");
                    else if (n.op == ">")  emit("setg", "al");
                    else if (n.op == ">=") emit("setge", "al");
                    emit("movzx", "rax, al");
                }
                else if (n.op == "&&") emit("and", "rax, rbx");
                else if (n.op == "||") emit("or", "rax, rbx");
            },
            [&](const UnaryExpr& n)
            {
                traverseAsmGenerator(*n.operand);
                if (n.op == "!")
                {
                    emit("test", "rax, rax");
                    emit("sete", "al");
                    emit("movzx", "rax, al");
                }
                else if (n.op == "-")
                {
                    emit("neg", "rax");
                }
                else
                {
                    throw std::runtime_error("Unrecognised unary operator: " + n.op);
                }
            },
            [&](const Assignment& n)
            {
                traverseAsmGenerator(*n.value); // Evaluate right side into rax
                if (n.mustDeclare || !_symbolTable.currentTable->contains(n.name))
                {
                    try
                    {
                        _symbolTable.currentTable->addLocalVariable(n.name, node.type);
                    }
                    catch (CompilerException e)
                    {
                        e.setLocation(n.location);
                        throw e;
                    }
                }

                _symbolTable.currentTable->ensureInitialised(n.name);
                auto offset = _symbolTable.currentTable->getStackOffset(n.name);
                mov("[rbp" + std::to_string(offset) + "]", "rax");
            },
            [&](const Declaration& n)
            {
                _symbolTable.currentTable->addLocalVariable(n.name, node.type, false);
            },
            [&](const PrintStmt& n)
            {
                _requiresPrint = true;
                traverseAsmGenerator(*n.value); // Evaluate expr into rax
                intToString();      // Convert rax to string in buffer
                
                // Setup Windows API call (WriteFile)
                // WriteFile(hFile, lpBuffer, nNumberOfBytesToWrite, lpNumberOfBytesWritten, lpOverlapped)
                // rcx=handle, rdx=buffer, r8=byteCount, r9=&bytes_written, [rsp+20h]=NULL
                mov("rdx", "rdi");      // rdi was set by intToString to the start of the string
                mov("r8", "rcx");       // rcx was set by intToString as the count
                mov("rcx", "[hStdOut]");
                emit("mov", "r9, OFFSET bytes_written");
                mov("qword ptr [rsp+20h]", "0"); // lpOverlapped = NULL (5th arg on stack)

                // Shadow space is already reserved in main's prologue
                emit("call", "WriteFile");
            },
            [&](const ScopeNode& n)
            {
                SymbolTable scopeSymbols{ &_symbolTable };
                for (auto& node : n.statements)
                    traverseAsmGenerator(*node);
            }
        }, node.data);
    }

    std::string Generator::generateFinal()
    {
        generateGlobalData();

        std::stringstream output;
        output << "option casemap:none\n";
        output << "includelib kernel32.lib\n";
        output << "EXTERN GetStdHandle:PROC\n";
        output << "EXTERN WriteFile:PROC\n";
        output << "EXTERN ExitProcess:PROC\n\n";

        output << ".data\n" << _dataSection.str() << "\n";

        output << ".code\n";
        output << "main PROC\n";

        setupStackFrame(output);
        if (_requiresPrint) getStdOutHandle(output);
        output << _codeSection.str();
        tearDownStackFrame(output);
        set0("ecx", output);
        emit("call", "ExitProcess", output);

        output << "main ENDP\n";
        output << "END\n";

        return output.str();
    }

    void Generator::generateGlobalData()
    {
        if (_requiresPrint)
        {
            _dataSection << "STD_OUTPUT_HANDLE equ -11\n";
            _dataSection << "hStdOut dq 0\n";
            _dataSection << "negative_flag db 0\n";
            _dataSection << "global_buffer db 24 dup(0)\n";
            _dataSection << "bytes_written dd 0\n";
        }
    }

    void Generator::intToString()
    {
        // Convert integer in RAX to string, filling in characters from the end
        // RDI: Current pos in buffer, RCX: chars written
        std::string labelId = std::to_string(_labelCounter++);
        std::string posLabel = "pos_" + labelId;
        std::string loopLabel = "conv_" + labelId;
        std::string endLabel = "done_" + labelId;

        // Reset negative flag
        mov("byte ptr [negative_flag]", "0");
        emit("cmp", "rax, 0");
        emit("jge", posLabel);

        // Handle negative
        emit("neg", "rax");
        mov("byte ptr [negative_flag]", "1");

        emit(posLabel + ":");
        emit("lea", "rdi, [global_buffer + 22]"); // Leave space for newline
        mov("rbx", "10");
        set0("rcx");
        mov("byte ptr [rdi + 1]", "0Ah");        // Place newline
        emit("inc", "rcx");

        // Conversion Loop
        emit(loopLabel + ":");
        set0("rdx");
        emit("div", "rbx");
        add("dl", "'0'");
        mov("[rdi]", "dl");
        emit("dec", "rdi");
        emit("inc", "rcx");
        emit("test", "rax, rax");
        emit("jnz", loopLabel);

        // Add '-' if negative
        emit("cmp", "byte ptr [negative_flag], 1");
        emit("jne", endLabel);
        mov("byte ptr [rdi]", "'-'");
        emit("dec", "rdi");
        emit("inc", "rcx");

        emit(endLabel + ":");
        emit("inc", "rdi"); // Points RDI to the first character to print
    }

    void Generator::getStdOutHandle(std::stringstream& target)
    {
        mov("rcx", "STD_OUTPUT_HANDLE", target);
        emit("call", "GetStdHandle", target);
        mov("[hStdOut]", "rax", target);
    }

    void Generator::setupStackFrame(std::stringstream& target)
    {
        push("rbp", target); // Save the old base pointer
        mov("rbp", "rsp", target); // Set the new base pointer to the current stack pointer
        push("rdi", target); // Save non-volatile registers
        push("rbx", target);

        int localsSpace = _symbolTable.getTotalStackSize();
        int totalSpace = 32 + localsSpace;
        if (totalSpace % 16 != 0)
            totalSpace += (16 - (totalSpace % 16));

        // Temp: Using fixed size buffer space - will need to calculate to fit locals
        sub("rsp", std::to_string(totalSpace), target);
        _lastLocalSpaceReserved = totalSpace;
    }

    void Generator::tearDownStackFrame(std::stringstream& target)
    {
        add("rsp", std::to_string(_lastLocalSpaceReserved), target); // Remove buffer space
        pop("rbx", target); // Restore non-volatile registers
        pop("rdi", target);
        pop("rbp", target);
    }

    void Generator::emit(const std::string& instr, const std::string& ops, std::stringstream& target)
    {
        target << "    " << std::left << std::setw(8) << instr << ops << "\n";
    }

    void Generator::push(const std::string& reg, std::stringstream& target)
    {
        emit("push", reg, target);
    }

    void Generator::pop(const std::string& reg, std::stringstream& target)
    {
        emit("pop", reg, target);
    }

    void Generator::mov(const std::string& dest, const std::string& src, std::stringstream& target)
    {
        emit("mov", dest + ", " + src, target);
    }

    void Generator::add(const std::string& dest, const std::string& src, std::stringstream& target)
    {
        emit("add", dest + ", " + src, target);
    }

    void Generator::sub(const std::string& dest, const std::string& src, std::stringstream& target)
    {
        emit("sub", dest + ", " + src, target);
    }

    void Generator::set0(const std::string& reg, std::stringstream& target)
    {
        emit("xor", reg + ", " + reg, target);
    }
}
